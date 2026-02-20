#pragma once
#include "System.hpp"

class HealthSystem : public System {
public:
    explicit HealthSystem() : System() {}
    void update(float dt) override;
};
