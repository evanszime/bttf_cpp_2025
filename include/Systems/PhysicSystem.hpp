#pragma once
#include "System.hpp"

class PhysicSystem : public System {
public:
    static constexpr float g = 0.f; // pas de gravité top-down
    explicit PhysicSystem() : System() {}
    void update(float dt) override;
    void applyForce(Entity e, float fx, float fy);
    void setVelocity(Entity e, float vx, float vy);
    void stop(Entity e);
};
