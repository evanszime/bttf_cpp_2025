#pragma once
#include "World.hpp"
#include <string>
#include <memory>
#include <unordered_map>

class InputContext;

enum class SceneType { SPLASH, MENU, GAME, SETTINGS, GAMEOVER, INFO };

class Scene {
protected:
    World        world;
    std::string  sceneName;
    bool         isActive = false;
public:
    explicit Scene(const std::string& name) : sceneName(name) {}
    virtual ~Scene() = default;
    virtual void onEnter() = 0;
    virtual void onExit()  = 0;
    virtual void update(float dt) = 0;
    virtual void render(float dt) = 0;
    void setActive(bool a) { isActive = a; }
    bool getActive() const { return isActive; }
    const std::string& getName() const { return sceneName; }
};

class SceneManager {
    std::unordered_map<SceneType, std::unique_ptr<Scene>> _scenes;
    Scene*    _current       = nullptr;
    SceneType _next          = SceneType::SPLASH;
    bool      _changePending = false;
public:
    void registerScene(SceneType type, std::unique_ptr<Scene> scene) {
        _scenes[type] = std::move(scene);
    }
    void changeScene(SceneType type) { _next=type; _changePending=true; }
    void processTransition() {
        if (!_changePending) return;
        _changePending = false;
        if (_current) { _current->onExit(); _current->setActive(false); }
        auto it = _scenes.find(_next);
        if (it != _scenes.end()) {
            _current = it->second.get();
            _current->setActive(true);
            _current->onEnter();
        }
    }
    void update(float dt) { processTransition(); if(_current&&_current->getActive()) _current->update(dt); }
    void render(float dt) { if(_current&&_current->getActive()) _current->render(dt); }
    bool hasScene(SceneType t) const { return _scenes.count(t)>0; }
};
