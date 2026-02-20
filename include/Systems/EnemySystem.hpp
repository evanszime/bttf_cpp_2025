#pragma once
#include "System.hpp"
#include "../GlobalComponent.hpp"
#include "../SceneManager.hpp"

class EnemySystem : public System {
private:
    GlobalComponent* _global = nullptr;
    SceneManager*    _sm     = nullptr;
public:
    explicit EnemySystem() : System() {}
    void setGlobal(GlobalComponent& g) { _global = &g; }
    void setSceneManager(SceneManager& sm) { _sm = &sm; }
    void update(float dt) override;

    // Spawn un ennemi normal ou boss
    void spawnEnemy(float x, float y, int wave, bool isBoss = false);
};
