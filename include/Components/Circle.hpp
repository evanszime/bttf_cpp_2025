#pragma once
#include <raylib.h>
#include "Sprite.hpp"
struct CircleComponent {
    float radius    = 10.f;
    float tickness  = 0.f;
    Color inlineColor  = RED;
    Color outlineColor = DARKGRAY;
    int   layer = 0;
    EntityTag tag = EntityTag::NONE;
};
