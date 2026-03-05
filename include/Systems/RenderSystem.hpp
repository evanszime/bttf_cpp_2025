#pragma once
#include "System.hpp"
#include "../GlobalComponent.hpp"
#include <raylib.h>
#include <string>

class RenderSystem : public System {
private:
    GlobalComponent* _global = nullptr;
    RenderTexture2D  _renderTarget = {};
    float            _renderScale  = 1.f;
    Vector2          _renderOffset = {0, 0};
    float            _camX = 0.f, _camY = 0.f;
    static const int VIRTUAL_W = 1920;
    static const int VIRTUAL_H = 1080;
public:
    explicit RenderSystem() = default;
    void setGlobal(GlobalComponent& g) { _global = &g; }
    void setCameraOffset(float cx, float cy) { _camX=cx; _camY=cy; }
    void initiateWindow(int w, int h, int fps, const std::string& t, bool full);
    void initRenderTarget();
    void update(float dt) override;
};
