#pragma once
#include "World.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

enum class SceneType {
    MENU,
    GAME,
    SETTINGS,
    GAME_OVER,
    VICTORY,
};

class Scene {
protected:
    World       world;
    std::string sceneName;
    bool        isActive = false;

public:
    Scene(const std::string& name) : sceneName(name) {}
    virtual ~Scene() = default;

    virtual void onEnter()          = 0;
    virtual void onExit()           = 0;
    virtual void update(float dt)   = 0;
    virtual void render(float dt)   = 0;

    World&       getWorld()      { return world; }
    const std::string& getName() const { return sceneName; }
    bool  getIsActive()          const { return isActive; }
    void  setActive(bool active)       { isActive = active; }
};

class SceneManager {
private:
    std::unordered_map<SceneType, std::unique_ptr<Scene>> scenes;
    Scene* currentScene   = nullptr;
    Scene* nextScene      = nullptr;
    bool   isTransitioning = false;

public:
    void registerScene(SceneType type, std::unique_ptr<Scene> scene) {
        scenes[type] = std::move(scene);
    }

    void changeScene(SceneType type) {
        auto it = scenes.find(type);
        if (it != scenes.end()) {
            nextScene       = it->second.get();
            isTransitioning = true;
            processTransition();
        }
    }

    void processTransition() {
        if (!isTransitioning || !nextScene) return;

        if (currentScene) {
            currentScene->onExit();
            currentScene->setActive(false);
            currentScene->getWorld().clearWorld();
        }

        currentScene = nextScene;
        currentScene->setActive(true);
        currentScene->onEnter();

        nextScene       = nullptr;
        isTransitioning = false;
    }

    void update(float dt) {
        if (currentScene && currentScene->getIsActive())
            currentScene->update(dt);
    }

    void render(float dt) {
        if (currentScene && currentScene->getIsActive())
            currentScene->render(dt);
    }

    Scene*  getCurrentScene()  { return currentScene; }
    World*  getCurrentWorld()  { return currentScene ? &currentScene->getWorld() : nullptr; }
    bool    isInTransition()   const { return isTransitioning; }
};
