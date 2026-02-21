#pragma once
#include "System.hpp"
#include "../GlobalComponent.hpp"
class AnimationSystem : public System {
private:
    GlobalComponent* _global = nullptr;
public:
    explicit AnimationSystem() = default;
    void setGlobal(GlobalComponent& g) { _global = &g; }
    void update(float dt) override;
};
