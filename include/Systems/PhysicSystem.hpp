#pragma once
#include "System.hpp"
static constexpr float g = 9.81f;
class PhysicSystem : public System {
public:
    explicit PhysicSystem() = default;
    void update(float dt) override;
    void applyForce(Entity e, float fx, float fy);
    void applyImpulse(Entity e, float ix, float iy);
    void setVelocity(Entity e, float vx, float vy);
    void stop(Entity e);
};
