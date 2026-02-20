#include "../include/Engine.hpp"
#include <cmath>

// Explosion AoE pour les roquettes
static void explodeRocket(World& world, float cx, float cy, float dmg, float radius)
{
    auto& positions = world.getContainer<PositionComponent>();
    auto& healths   = world.getContainer<HealthComponent>();
    auto& sprites   = world.getContainer<SpriteComponent>();

    for (Entity e : sprites.getEntities()) {
        auto* spr = sprites.get(e);
        if (!spr) continue;
        if (spr->tag != EntityTag::ENEMY && spr->tag != EntityTag::BOSS) continue;
        auto* pos = positions.get(e);
        if (!pos) continue;
        float dx = pos->x - cx, dy = pos->y - cy;
        if (sqrtf(dx*dx + dy*dy) <= radius) {
            if (auto* h = healths.get(e))
                h->hp -= dmg;
        }
    }
}

void BulletSystem::update(float dt)
{
    if (!world) return;

    auto& bullets   = world->getContainer<BulletComponent>();
    auto& positions = world->getContainer<PositionComponent>();
    auto& sprites   = world->getContainer<SpriteComponent>();
    auto& colliders = world->getContainer<BoxColliderComponent>();
    auto& healths   = world->getContainer<HealthComponent>();

    std::vector<Entity> toRemove;

    for (Entity e : bullets.getEntities()) {
        auto* b   = bullets.get(e);
        auto* pos = positions.get(e);
        if (!b || !pos) continue;

        // Lifetime
        b->elapsed += dt;
        if (b->elapsed >= b->lifetime) {
            // Rocket: explosion à expiration aussi
            if (b->pierceLeft == -99)
                explodeRocket(*world, pos->x, pos->y, b->damage, 80.f);
            toRemove.push_back(e);
            continue;
        }

        // Vérifier collisions avec ennemis
        auto* col = colliders.get(e);
        if (!col) continue;

        bool destroyed = false;
        std::vector<int> collidedCopy = col->EntityCollided;

        for (int otherId : collidedCopy) {
            Entity other = static_cast<Entity>(otherId);
            auto* otherSpr = sprites.get(other);
            if (!otherSpr) continue;
            if (otherSpr->tag != EntityTag::ENEMY && otherSpr->tag != EntityTag::BOSS) continue;

            // Rocket: AoE à l'impact
            if (b->pierceLeft == -99) {
                explodeRocket(*world, pos->x, pos->y, b->damage, 80.f);
                toRemove.push_back(e);
                destroyed = true;
                break;
            }

            // Dégâts normaux
            if (auto* h = healths.get(other))
                h->hp -= b->damage;

            if (b->pierce && b->pierceLeft > 0) {
                b->pierceLeft--;
                // continue sur d'autres cibles
            } else {
                toRemove.push_back(e);
                destroyed = true;
                break;
            }
        }

        if (destroyed) continue;

        // Out of bounds (arène 1920x1080)
        if (pos->x < -50 || pos->x > 1970 || pos->y < -50 || pos->y > 1130) {
            toRemove.push_back(e);
        }
    }

    for (Entity e : toRemove)
        world->removeEntity(e);
}
