#pragma once
#include "System.hpp"
#include "../SceneManager.hpp"
#include <raylib.h>

class InputSystem : public System {
private:
    SceneManager* _sceneManager = nullptr;

public:
    explicit InputSystem() : System() {}
    void setSceneManager(SceneManager& sm) { _sceneManager = &sm; }
    void update(float dt) override;
};
