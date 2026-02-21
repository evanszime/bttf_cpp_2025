#pragma once
#include <raylib.h>
#include <string>
struct TextComponent {
    std::string text      = "";
    float       size      = 20.f;
    Color       font_color = WHITE;
    bool        isTaping  = false;
    int         maxChars  = 100;
    int         layer     = 9;
    bool        field     = false;
    bool        visible   = true;
};
