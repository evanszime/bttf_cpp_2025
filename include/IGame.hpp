#pragma once
#include "Engine.hpp"

class IGame {
public:
    struct Resolution { int width=1280,height=720,fps=60; std::string title="Zimex"; bool full=false; };

protected:
    Resolution   resolution;
    SceneManager sceneManager;
    bool         isRunning = false;

public:
    bool exit = false;

    IGame(int w, int h, int fps, const std::string& title, bool full)
        : resolution{w, h, fps, title, full} {}
    virtual ~IGame() = default;

    virtual void loadAssets()       = 0;
    virtual void initializeScenes() = 0;
    virtual void update(float dt)   = 0;
    virtual void render(float dt)   = 0;
    virtual void close()            = 0;

    virtual void run() {
        InitWindow(resolution.width, resolution.height, resolution.title.c_str());
        if (resolution.full) ToggleFullscreen();
        SetTargetFPS(resolution.fps);
        InitAudioDevice();
        SetMasterVolume(0.5f);
        SetExitKey(KEY_NULL);

        loadAssets();
        initializeScenes();
        sceneManager.changeScene(SceneType::SPLASH);
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

    SceneManager& getSceneManager() { return sceneManager; }
    const Resolution& getResolution() const { return resolution; }
};
