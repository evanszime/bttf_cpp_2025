#pragma once
#include "System.hpp"
#include "../SceneManager.hpp"
#include "../GlobalComponent.hpp"

class WaveSystem : public System {
private:
    SceneManager*    _sm     = nullptr;
    GlobalComponent* _global = nullptr;
public:
    explicit WaveSystem() : System() {}
    void setSceneManager(SceneManager& sm) { _sm = &sm; }
    void setGlobal(GlobalComponent& g)     { _global = &g; }
    void update(float dt) override;
};
