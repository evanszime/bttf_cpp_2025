#pragma once
#include "System.hpp"
#include "../GlobalComponent.hpp"
class SoundSystem : public System {
private:
    GlobalComponent* _global = nullptr;
public:
    explicit SoundSystem() = default;
    void setGlobal(GlobalComponent& g) { _global = &g; }
    void update(float dt) override;
};
