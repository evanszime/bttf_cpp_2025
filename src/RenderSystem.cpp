#include "../include/Engine.hpp"
#include <algorithm>

static const int VIRTUAL_W = 1920;
static const int VIRTUAL_H = 1080;

void RenderSystem::initRenderTarget()
{
    _renderTarget = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
}

void RenderSystem::update(float dt)
{
    if (!world || !_global) return;

    auto& sprites    = world->getContainer<SpriteComponent>();
    auto& positions  = world->getContainer<PositionComponent>();
    auto& texts      = world->getContainer<TextComponent>();

    struct Drawable {
        Entity e;
        int    layer;
        enum Type { SPRITE, TEXT } type;
    };

    std::vector<Drawable> drawables;

    for (Entity e : sprites.getEntities()) {
        auto* s = sprites.get(e);
        if (s && s->visible)
            drawables.push_back({ e, s->layer, Drawable::SPRITE });
    }
    for (Entity e : texts.getEntities()) {
        auto* t = texts.get(e);
        if (t && t->visible)
            drawables.push_back({ e, t->layer, Drawable::TEXT });
    }

    std::sort(drawables.begin(), drawables.end(),
        [](const Drawable& a, const Drawable& b) { return a.layer < b.layer; });

    // Calcul mise à l'échelle virtuelle
    float scaleX = (float)GetScreenWidth()  / VIRTUAL_W;
    float scaleY = (float)GetScreenHeight() / VIRTUAL_H;
    _renderScale = (scaleX < scaleY) ? scaleX : scaleY;
    _renderOffset.x = (GetScreenWidth()  - VIRTUAL_W * _renderScale) / 2.f;
    _renderOffset.y = (GetScreenHeight() - VIRTUAL_H * _renderScale) / 2.f;

    BeginTextureMode(_renderTarget);
    ClearBackground({ 20, 20, 30, 255 }); // fond sombre Brotato-style

    for (auto& d : drawables) {
        auto* pos = positions.get(d.e);
        if (!pos) continue;

        float px = pos->x;
        float py = pos->y;

        if (d.type == Drawable::SPRITE) {
            auto* s = sprites.get(d.e);
            if (!s) continue;
            if ((int)s->texturePath >= (int)_global->_allSprites.size()) continue;

            Texture2D& tex = _global->_allSprites[s->texturePath];

            // Si textureRect non défini (width==0), utiliser toute la texture
            Rectangle src;
            if (s->width == 0 || s->height == 0) {
                src = { s->left, s->top, (float)tex.width, (float)tex.height };
            } else {
                src = { s->left, s->top, s->width, s->height };
            }

            float dw = (s->width  > 0 ? s->width  : tex.width)  * s->scale;
            float dh = (s->height > 0 ? s->height : tex.height) * s->scale;

            Rectangle dst = {
                px + s->offsetX,
                py + s->offsetY,
                dw, dh
            };
            Vector2 origin = { dw / 2.f, dh / 2.f };

            DrawTexturePro(tex, src, dst, origin, s->rotation, s->tint);

        } else if (d.type == Drawable::TEXT) {
            auto* t = texts.get(d.e);
            if (!t || t->size <= 0) continue;
            if (t->field) {
                int tw = MeasureText(t->text.c_str(), t->size);
                DrawRectangle((int)px - 5, (int)py - 5, tw + 10, t->size + 10,
                    t->isTaping ? RED : BLACK);
            }
            DrawText(t->text.c_str(), (int)px, (int)py, t->size, t->font_color);
        }
    }

    EndTextureMode();

    // Upscale vers la fenêtre réelle
    Rectangle src = { 0, 0, (float)_renderTarget.texture.width,
                            -(float)_renderTarget.texture.height };
    Rectangle dst = { _renderOffset.x, _renderOffset.y,
                      VIRTUAL_W * _renderScale, VIRTUAL_H * _renderScale };
    DrawTexturePro(_renderTarget.texture, src, dst, { 0, 0 }, 0.f, WHITE);
}
