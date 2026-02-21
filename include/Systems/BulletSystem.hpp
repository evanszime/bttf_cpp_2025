#pragma once
#include "System.hpp"
class BulletSystem : public System {
public:
    explicit BulletSystem() = default;
    void update(float dt) override;
};
