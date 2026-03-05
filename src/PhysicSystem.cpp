#include "../include/Engine.hpp"

void PhysicSystem::update(float dt) {
    auto& positions   = world->getContainer<PositionComponent>();
    auto& rigidbodies = world->getContainer<RigidbodyComponent>();
    for (Entity e : rigidbodies.getEntities()) {
        auto* rb  = rigidbodies.get(e);
        auto* pos = positions.get(e);
        if (!rb || !pos || rb->isStatic) continue;
        if (rb->useGravity) rb->fy += g * rb->mass;
        rb->vx += (rb->fx / rb->mass) * dt;
        rb->vy += (rb->fy / rb->mass) * dt;
        pos->x += rb->vx * dt;
        pos->y += rb->vy * dt;
        rb->fx = 0; rb->fy = 0;
    }
}
void PhysicSystem::applyForce(Entity e, float fx, float fy) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) { rb->fx+=fx; rb->fy+=fy; }
}

void PhysicSystem::applyImpulse(Entity e, float ix, float iy) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) { rb->vx+=ix/rb->mass; rb->vy+=iy/rb->mass; }
}

void PhysicSystem::setVelocity(Entity e, float vx, float vy) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) { rb->vx=vx; rb->vy=vy; }
}

void PhysicSystem::stop(Entity e) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) rb->vx=rb->vy=rb->fx=rb->fy=0;
}

