#pragma once
#include "IGame.hpp"
#include "Scenes.hpp"
#include "GlobalComponent.hpp"

class Game : public IGame {
public:
    Game() : IGame(1280, 720, 60, "ZIMEX", false) {}

    void loadAssets() override;
    void initializeScenes() override;
    void update(float dt) override { sceneManager.update(dt); }
    void render(float dt) override { sceneManager.render(dt); }
    void close() override {}
};
