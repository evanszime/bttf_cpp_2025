#include "../include/Engine.hpp"

void HealthSystem::update(float dt)
{
    if (!world) return;
    auto& healths = world->getContainer<HealthComponent>();

    std::vector<Entity> toRemove;
    for (Entity e : healths.getEntities()) {
        auto* h = healths.get(e);
        if (!h) continue;
        h->hp = std::min(h->hp, h->maxHp);
        if (h->hp <= (float)h->deathHp && !h->isDead) {
            h->isDead = true;
            toRemove.push_back(e);
        }
    }
    for (Entity e : toRemove)
        world->removeEntity(e);
}
