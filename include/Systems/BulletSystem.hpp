#pragma once
#include "System.hpp"

class BulletSystem : public System {
public:
    explicit BulletSystem() : System() {}
    void update(float dt) override;
};
