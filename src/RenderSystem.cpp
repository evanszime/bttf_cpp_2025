#include "../include/Engine.hpp"
#include <algorithm>

static const int VIRTUAL_W = 1920;
static const int VIRTUAL_H = 1080;

void RenderSystem::initiateWindow(int w, int h, int fps, const std::string& t, bool full) {
    InitWindow(w, h, t.c_str());
    if (full) ToggleFullscreen();
    SetTargetFPS(fps);
    initRenderTarget();
}

void RenderSystem::initRenderTarget() {
    if (_renderTarget.id != 0) UnloadRenderTexture(_renderTarget);
    _renderTarget = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
}

void RenderSystem::update(float dt) {
    (void)dt;
    if (!world || !_global) return;

    auto& sprites    = world->getContainer<SpriteComponent>();
    auto& positions  = world->getContainer<PositionComponent>();
    auto& circles    = world->getContainer<CircleComponent>();
    auto& rectangles = world->getContainer<RectangleComponent>();
    auto& texts      = world->getContainer<TextComponent>();

    struct Drawable {
        Entity e; int layer;
        enum Type { SPRITE, CIRCLE, RECT, TEXT } type;
    };
    std::vector<Drawable> drawables;
    for (Entity e : sprites.getEntities())
        if (auto* s = sprites.get(e)) drawables.push_back({e, s->layer, Drawable::SPRITE});
    for (Entity e : circles.getEntities())
        if (auto* c = circles.get(e)) drawables.push_back({e, c->layer, Drawable::CIRCLE});
    for (Entity e : rectangles.getEntities())
        if (auto* r = rectangles.get(e)) drawables.push_back({e, r->layer, Drawable::RECT});
    for (Entity e : texts.getEntities())
        if (auto* t = texts.get(e)) drawables.push_back({e, t->layer, Drawable::TEXT});

    std::stable_sort(drawables.begin(), drawables.end(),
        [](const Drawable& a, const Drawable& b){ return a.layer < b.layer; });

    float scaleX = (float)GetScreenWidth()  / VIRTUAL_W;
    float scaleY = (float)GetScreenHeight() / VIRTUAL_H;
    _renderScale  = (scaleX < scaleY) ? scaleX : scaleY;
    _renderOffset.x = (GetScreenWidth()  - VIRTUAL_W * _renderScale) / 2.f;
    _renderOffset.y = (GetScreenHeight() - VIRTUAL_H * _renderScale) / 2.f;

    BeginTextureMode(_renderTarget);
        ClearBackground(BLACK);
        for (auto& d : drawables) {
            auto* pos = positions.get(d.e);
            if (!pos) continue;
            // Appliquer l'offset caméra : décaler dans le monde virtuel
            float px = pos->x - _camX;
            float py = pos->y - _camY;

            // Frustum culling : hors de l'espace virtuel → skip
            if (d.type == Drawable::SPRITE) {
                auto* s = sprites.get(d.e);
                if (s) {
                    float sw2=s->width*s->scale, sh2=s->height*s->scale;
                    if (px+sw2<-50||px-sw2>VIRTUAL_W+50||py+sh2<-50||py-sh2>VIRTUAL_H+50) continue;
                }
            }

            switch (d.type) {
            case Drawable::SPRITE: {
                auto* s = sprites.get(d.e);
                if (s && (int)_global->_allSprites.size() > s->texturePath) {
                    // Filtre POINT pour la map (pixel art net, pas de flou)
                    if (s->texturePath == 36) {
                        SetTextureFilter(_global->_allSprites[36], TEXTURE_FILTER_POINT);
                    }
                    // Cas spécial : la MAP est immense, on la centre sur la caméra
                    DrawTexturePro(
                        _global->_allSprites[s->texturePath],
                        // Source : on lit la portion de la map visible
                        (s->texturePath == 36)
                            ? Rectangle{_camX, _camY, (float)VIRTUAL_W, (float)VIRTUAL_H}
                            : Rectangle{s->left, s->top, s->width, s->height},
                        // Dest : pour la map c'est le plein écran virtuel, sinon position monde
                        (s->texturePath == 36)
                            ? Rectangle{0, 0, (float)VIRTUAL_W, (float)VIRTUAL_H}
                            : Rectangle{px+s->offsetX, py+s->offsetY, s->width*s->scale, s->height*s->scale},
                        (s->texturePath == 36)
                            ? Vector2{0,0}
                            : Vector2{(s->width*s->scale)/2.f,(s->height*s->scale)/2.f},
                        s->rotation,
                        s->tint
                    );
                }
                break;
            }
            case Drawable::TEXT: {
                auto* t = texts.get(d.e);
                if (!t||!t->visible) break;
                DrawText(t->text.c_str(),(int)px,(int)py,(int)t->size,t->font_color);
                break;
            }
            case Drawable::CIRCLE: {
                auto* c = circles.get(d.e);
                if (!c) break;
                if (c->tickness>0) DrawCircleLines((int)px,(int)py,c->radius,c->outlineColor);
                DrawCircle((int)px,(int)py,c->radius,c->inlineColor);
                break;
            }
            case Drawable::RECT: {
                auto* r = rectangles.get(d.e);
                if (!r) break;
                if (r->tickness>0) DrawRectangleLines((int)px,(int)py,r->width,r->height,r->outlineColor);
                DrawRectangle((int)px,(int)py,r->width,r->height,r->inlineColor);
                break;
            }
            }
        }
    EndTextureMode();

    Rectangle src = {0,0,(float)_renderTarget.texture.width,-(float)_renderTarget.texture.height};
    Rectangle dst = {_renderOffset.x,_renderOffset.y,VIRTUAL_W*_renderScale,VIRTUAL_H*_renderScale};
    DrawTexturePro(_renderTarget.texture, src, dst, {0,0}, 0.f, WHITE);
}

