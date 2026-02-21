#pragma once
#include "System.hpp"
#include "../GlobalComponent.hpp"
class WeaponSystem : public System {
private:
    GlobalComponent* _global = nullptr;
public:
    explicit WeaponSystem() = default;
    void setGlobal(GlobalComponent& g) { _global = &g; }
    void update(float dt) override;
};
