#pragma once
#include <raylib.h>
#include "Sprite.hpp"
struct RectangleComponent {
    float width    = 10.f;
    float height   = 10.f;
    float tickness = 0.f;
    Color inlineColor  = BLUE;
    Color outlineColor = DARKBLUE;
    int   layer = 0;
    EntityTag tag = EntityTag::NONE;
};
