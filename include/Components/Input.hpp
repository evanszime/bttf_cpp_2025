#pragma once
#include <unordered_map>
#include <functional>
#include <raylib.h>

// Marqueur simple - l'entité est contrôlée par le joueur
struct InputComponent {
    bool isPlayer = true;
};
