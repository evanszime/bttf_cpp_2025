#include "../include/Engine.hpp"

void PhysicSystem::update(float dt)
{
    if (!world) return;
    auto& positions   = world->getContainer<PositionComponent>();
    auto& rigidbodies = world->getContainer<RigidbodyComponent>();

    for (Entity e : rigidbodies.getEntities()) {
        auto* rb  = rigidbodies.get(e);
        auto* pos = positions.get(e);
        if (!rb || !pos || rb->isStatic) continue;
        pos->x += rb->vx * dt;
        pos->y += rb->vy * dt;
    }
}

void PhysicSystem::applyForce(Entity e, float fx, float fy) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) {
        rb->fx += fx; rb->fy += fy;
    }
}

void PhysicSystem::setVelocity(Entity e, float vx, float vy) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) {
        rb->vx = vx; rb->vy = vy;
    }
}

void PhysicSystem::stop(Entity e) {
    if (auto* rb = world->getComponent<RigidbodyComponent>(e)) {
        rb->vx = rb->vy = rb->fx = rb->fy = 0.f;
    }
}
