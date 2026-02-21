#include "../include/Engine.hpp"
void HealthSystem::update(float dt) {
    (void)dt;
    auto& healths = world->getContainer<HealthComponent>();
    for (Entity e : healths.getEntities()) {
        auto* h = healths.get(e); if (!h) continue;
        if (h->hp > h->maxHp) h->hp = h->maxHp;
        if (h->hp <= h->deathHp && !h->isDead) h->isDead = true;
    }
}
