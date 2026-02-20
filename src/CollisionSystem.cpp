#include "../include/Engine.hpp"
#include <cmath>

struct LayerTagInfo {
    int       layer = 0;
    EntityTag tag   = EntityTag::NONE;
    bool      valid = false;
};

static LayerTagInfo getInfo(Entity e, World& world) {
    LayerTagInfo info;
    auto* spr = world.getComponent<SpriteComponent>(e);
    if (!spr) return info;
    info.layer = spr->layer;
    info.tag   = spr->tag;
    info.valid = true;
    return info;
}

static bool canCollide(const BoxColliderComponent& a, const LayerTagInfo& bInfo) {
    if (!a.TagsCollided.empty()) {
        for (int t : a.TagsCollided)
            if (t == (int)bInfo.tag) return true;
        return false;
    }
    return true;
}

static bool checkAABB(Entity ea, Entity eb, World& world,
                      const BoxColliderComponent& a, const BoxColliderComponent& b) {
    auto* pa = world.getComponent<PositionComponent>(ea);
    auto* pb = world.getComponent<PositionComponent>(eb);
    if (!pa || !pb) return false;
    return (pa->x - a.width/2.f < pb->x + b.width/2.f  &&
            pa->x + a.width/2.f > pb->x - b.width/2.f  &&
            pa->y - a.height/2.f < pb->y + b.height/2.f &&
            pa->y + a.height/2.f > pb->y - b.height/2.f);
}

static bool checkCircles(Entity ea, Entity eb, World& world,
                         const BoxColliderComponent& a, const BoxColliderComponent& b) {
    auto* pa = world.getComponent<PositionComponent>(ea);
    auto* pb = world.getComponent<PositionComponent>(eb);
    if (!pa || !pb) return false;
    float dx = pa->x - pb->x, dy = pa->y - pb->y;
    return sqrtf(dx*dx + dy*dy) < (a.radius + b.radius);
}

static bool checkCircleRect(Entity circ, Entity rect, World& world,
                             const BoxColliderComponent& c, const BoxColliderComponent& r) {
    auto* pc = world.getComponent<PositionComponent>(circ);
    auto* pr = world.getComponent<PositionComponent>(rect);
    if (!pc || !pr) return false;
    float nx = std::max(pr->x - r.width/2.f,  std::min(pc->x, pr->x + r.width/2.f));
    float ny = std::max(pr->y - r.height/2.f, std::min(pc->y, pr->y + r.height/2.f));
    float dx = pc->x - nx, dy = pc->y - ny;
    return sqrtf(dx*dx + dy*dy) < c.radius;
}

void CollisionSystem::update(float dt)
{
    if (!world) return;
    auto& colliders = world->getContainer<BoxColliderComponent>();

    for (Entity e : colliders.getEntities())
        if (auto* c = colliders.get(e)) c->EntityCollided.clear();

    std::vector<Entity> entities = colliders.getEntities();

    for (size_t a = 0; a < entities.size(); ++a) {
        Entity ea  = entities[a];
        auto*  boxA = colliders.get(ea);
        if (!boxA) continue;
        LayerTagInfo infoA = getInfo(ea, *world);
        if (!infoA.valid) continue;

        for (size_t b = a + 1; b < entities.size(); ++b) {
            Entity eb   = entities[b];
            auto*  boxB  = colliders.get(eb);
            if (!boxB) continue;
            LayerTagInfo infoB = getInfo(eb, *world);
            if (!infoB.valid) continue;

            bool aColB = canCollide(*boxA, infoB);
            bool bColA = canCollide(*boxB, infoA);
            if (!aColB && !bColA) continue;

            bool hit = false;
            if      (boxA->isCircle &&  boxB->isCircle) hit = checkCircles(ea, eb, *world, *boxA, *boxB);
            else if (!boxA->isCircle && !boxB->isCircle) hit = checkAABB(ea, eb, *world, *boxA, *boxB);
            else if (boxA->isCircle)                     hit = checkCircleRect(ea, eb, *world, *boxA, *boxB);
            else                                         hit = checkCircleRect(eb, ea, *world, *boxB, *boxA);

            if (hit) {
                if (aColB) boxA->EntityCollided.push_back((int)eb);
                if (bColA) boxB->EntityCollided.push_back((int)ea);
            }
        }
    }
}

std::pair<bool, Entity> CollisionSystem::ifCollideTag(Entity e, EntityTag tag)
{
    if (!world) return {false, Entity(0)};
    auto* col = world->getComponent<BoxColliderComponent>(e);
    if (!col) return {false, Entity(0)};
    for (int id : col->EntityCollided) {
        Entity other = static_cast<Entity>(id);
        auto* spr = world->getComponent<SpriteComponent>(other);
        if (spr && spr->tag == tag) return {true, other};
    }
    return {false, Entity(0)};
}
