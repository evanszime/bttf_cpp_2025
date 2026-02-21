#pragma once
#include "System.hpp"
class HealthSystem : public System {
public:
    explicit HealthSystem() = default;
    void update(float dt) override;
};
