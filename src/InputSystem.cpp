#include "../include/Engine.hpp"
#include <cmath>

void InputSystem::update(float dt)
{
    if (!world) return;

    auto& inputs   = world->getContainer<InputComponent>();
    auto& positions= world->getContainer<PositionComponent>();
    auto& rigids   = world->getContainer<RigidbodyComponent>();
    auto& players  = world->getContainer<PlayerComponent>();
    auto& weapons  = world->getContainer<WeaponComponent>();

    for (Entity e : inputs.getEntities()) {
        auto* inp = inputs.get(e);
        if (!inp || !inp->isPlayer) continue;

        auto* pos  = positions.get(e);
        auto* rb   = rigids.get(e);
        auto* pl   = players.get(e);
        auto* wc   = weapons.get(e);
        if (!pos || !rb || !pl) continue;

        // === MOUVEMENT WASD ===
        float speed = pl->speed;
        float vx = 0.f, vy = 0.f;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    vy -= speed;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  vy += speed;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  vx -= speed;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) vx += speed;

        // Diagonale normalisée
        if (vx != 0.f && vy != 0.f) {
            vx *= 0.7071f;
            vy *= 0.7071f;
        }
        rb->vx = vx;
        rb->vy = vy;

        // === ORIENTATION VERS LA SOURIS ===
        Vector2 mouse = GetMousePosition();
        // Conversion mouse -> coordonnées virtuelles (RenderSystem scale)
        // Pour simplifier, on utilise les coords écran directement
        float dx = mouse.x - pos->x;
        float dy = mouse.y - pos->y;
        pl->lookAngle = atan2f(dy, dx) * RAD2DEG;

        // Mettre à jour la rotation du sprite
        auto* spr = world->getComponent<SpriteComponent>(e);
        if (spr) spr->rotation = pl->lookAngle;

        // === TIR - Clic gauche ===
        if (wc && wc->currentWeapon) {
            wc->currentWeapon->update(dt);
            if (wc->secondWeapon) wc->secondWeapon->update(dt);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float tx = mouse.x;
                float ty = mouse.y;
                wc->currentWeapon->fire(pos->x, pos->y, tx, ty);
            }

            // === SWAP ARME - scroll ou clic droit ===
            float wheel = GetMouseWheelMove();
            if ((wheel != 0.f || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && wc->secondWeapon) {
                std::swap(wc->currentWeapon, wc->secondWeapon);
            }

            // === RELOAD - touche R ===
            if (IsKeyPressed(KEY_R) && wc->currentWeapon) {
                wc->currentWeapon->reload();
            }

            // Sync HUD
            if (wc->currentWeapon) {
                wc->ammo      = wc->currentWeapon->getCurrentAmmo();
                wc->maxAmmo   = wc->currentWeapon->getMaxAmmo();
                wc->weaponName= wc->currentWeapon->getName();
            }
        }

        // === INVINCIBILITÉ (clignotement après hit) ===
        if (pl->isInvincible) {
            pl->invincTimer -= dt;
            if (pl->invincTimer <= 0.f) {
                pl->invincTimer  = 0.f;
                pl->isInvincible = false;
                if (spr) spr->tint = WHITE;
            } else {
                // Clignotement
                float blink = fmodf(pl->invincTimer, 0.2f);
                if (spr) spr->tint = (blink > 0.1f) ? WHITE : Fade(WHITE, 0.2f);
            }
        }
    }
}
