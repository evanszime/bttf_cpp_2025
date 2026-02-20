#pragma once
#include <string>
#include <raylib.h>

struct TextComponent {
    std::string text     = "";
    int         size     = 20;
    Color       font_color = WHITE;
    int         layer    = 9;
    bool        visible  = true;
    bool        isTaping = false; // champ de saisie actif
    bool        field    = false; // afficher le fond de champ
};
