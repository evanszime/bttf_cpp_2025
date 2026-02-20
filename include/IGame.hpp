#pragma once
#include "SceneManager.hpp"
#include "GlobalComponent.hpp"
#include <string>
#include <raylib.h>

class IGame {
public:
    struct Resolution {
        int         width;
        int         height;
        int         fps;
        std::string title;
        bool        full;
    };

protected:
    Resolution    resolution;
    SceneManager  sceneManager;
    bool          isRunning = false;

public:
    bool exit = false;

    IGame(int w, int h, int fps, const std::string& title, bool full)
        : resolution{w, h, fps, title, full} {}

    virtual ~IGame() = default;

    // À implémenter dans Game.cpp
    virtual void initializeScenes() = 0;
    virtual void loadAssets()       = 0;    // charge textures + sons
    virtual void update(float dt)   = 0;
    virtual void render(float dt)   = 0;
    virtual void close()            = 0;

    virtual void run() {
        InitWindow(resolution.width, resolution.height, resolution.title.c_str());
        if (resolution.full) ToggleFullscreen();
        SetTargetFPS(resolution.fps);
        InitAudioDevice();
        SetExitKey(KEY_NULL);

        loadAssets();
        initializeScenes();
        sceneManager.changeScene(SceneType::MENU);

        isRunning = true;

        while (!exit && !WindowShouldClose()) {
            float dt = GetFrameTime();

            update(dt);

            BeginDrawing();
            ClearBackground(BLACK);
            render(dt);
            EndDrawing();
        }

        close();
        CloseAudioDevice();
        CloseWindow();
    }

    const Resolution& getResolution() const { return resolution; }
    SceneManager&     getSceneManager()     { return sceneManager; }
};
