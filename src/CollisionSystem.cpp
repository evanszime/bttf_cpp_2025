#include "../include/Engine.hpp"
#include <cmath>
#include <algorithm>

struct LayerTagInfo { int layer=0; EntityTag tag=EntityTag::NONE; bool valid=false; };


static LayerTagInfo getInfo(Entity e, World& w) {
    LayerTagInfo info;
    if (auto* s = w.getComponent<SpriteComponent>(e)) { info.layer=s->layer; info.tag=s->tag; info.valid=true; }
    else if (auto* c = w.getComponent<CircleComponent>(e)) { info.layer=c->layer; info.tag=c->tag; info.valid=true; }
    else if (auto* r = w.getComponent<RectangleComponent>(e)) { info.layer=r->layer; info.tag=r->tag; info.valid=true; }
    return info;
}

static bool canCollide(const BoxColliderComponent& col, const LayerTagInfo& b) {
    if (!b.valid) return false;
    bool layerOk = col.layersCollided.empty() ||
        std::find(col.layersCollided.begin(), col.layersCollided.end(), b.layer) != col.layersCollided.end();
    bool tagOk = col.TagsCollided.empty() ||
        std::find(col.TagsCollided.begin(), col.TagsCollided.end(), (int)b.tag) != col.TagsCollided.end();
    return layerOk && tagOk;
}

static bool rectRect(Entity A, Entity B, World& w, const BoxColliderComponent& bA, const BoxColliderComponent& bB) {
    auto* pA = w.getComponent<PositionComponent>(A);
    auto* pB = w.getComponent<PositionComponent>(B);
    if (!pA || !pB) return false;
    return pA->x < pB->x+bB.width && pA->x+bA.width > pB->x &&
           pA->y < pB->y+bB.height && pA->y+bA.height > pB->y;
}

static bool circCirc(Entity A, Entity B, World& w, const BoxColliderComponent& bA, const BoxColliderComponent& bB) {
    auto* pA = w.getComponent<PositionComponent>(A);
    auto* pB = w.getComponent<PositionComponent>(B);
    if (!pA || !pB) return false;
    float dx = pA->x - pB->x, dy = pA->y - pB->y;
    float dist = std::sqrt(dx*dx + dy*dy);
    return dist < (bA.radius + bB.radius);
}

static bool rectCirc(Entity R, Entity C, World& w, const BoxColliderComponent& bR, const BoxColliderComponent& bC) {
    auto* pR = w.getComponent<PositionComponent>(R);
    auto* pC = w.getComponent<PositionComponent>(C);
    if (!pR || !pC) return false;
    float cx = pC->x, cy = pC->y;
    float nearX = std::max(pR->x, std::min(cx, pR->x + bR.width));
    float nearY = std::max(pR->y, std::min(cy, pR->y + bR.height));
    float dx = cx - nearX, dy = cy - nearY;
    return (dx*dx + dy*dy) < (bC.radius * bC.radius);
}

void CollisionSystem::update(float dt) {
    (void)dt;
    if (!world) return;
    auto& colliders = world->getContainer<BoxColliderComponent>();
    for (Entity e : colliders.getEntities())
        if (auto* c = colliders.get(e)) c->EntityCollided.clear();

    std::vector<Entity> entities = colliders.getEntities();
    for (size_t a = 0; a < entities.size(); a++) {
        Entity eA = entities[a];
        auto* cA = colliders.get(eA); if (!cA) continue;
        LayerTagInfo iA = getInfo(eA, *world); if (!iA.valid) continue;

        for (size_t b = a+1; b < entities.size(); b++) {
            Entity eB = entities[b];
            auto* cB = colliders.get(eB); if (!cB) continue;
            LayerTagInfo iB = getInfo(eB, *world); if (!iB.valid) continue;

            bool aColB = canCollide(*cA, iB);
            bool bColA = canCollide(*cB, iA);
            if (!aColB && !bColA) continue;

            bool col = false;
            if (cA->isCircle && cB->isCircle)  col = circCirc(eA,eB,*world,*cA,*cB);
            else if (!cA->isCircle && !cB->isCircle) col = rectRect(eA,eB,*world,*cA,*cB);
            else if (cA->isCircle)  col = rectCirc(eB,eA,*world,*cB,*cA);
            else                    col = rectCirc(eA,eB,*world,*cA,*cB);

            if (col) {
                if (aColB) cA->EntityCollided.push_back((int)eB);
                if (bColA) cB->EntityCollided.push_back((int)eA);
            }
        }
    }
}

std::pair<bool,Entity> CollisionSystem::ifCollideTag(Entity e, int tag) {
    if (!world) return {false, Entity(0)};
    auto* col = world->getComponent<BoxColliderComponent>(e);
    if (!col) return {false, Entity(0)};
    for (int id : col->EntityCollided) {
        Entity other = static_cast<Entity>(id);
        auto info = getInfo(other, *world);
        if ((int)info.tag == tag) return {true, other};
    }
    return {false, Entity(0)};
}
