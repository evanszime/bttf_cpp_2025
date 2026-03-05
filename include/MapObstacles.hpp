#pragma once
#include <vector>
#include <raylib.h>

// Map en 4K (3840x2160) — couvre exactement 2x le viewport virtuel (1920x1080)
static constexpr int MAP_WORLD_W = 3840;
static constexpr int MAP_WORLD_H = 2160;

// Pas d'obstacles codés — collisions gérées par les bords map
static const std::vector<Rectangle> MAP_OBSTACLES = {};

// Spawn center = milieu de la map
static constexpr float SPAWN_CENTER_X = MAP_WORLD_W / 2.f;  // 1920
static constexpr float SPAWN_CENTER_Y = MAP_WORLD_H / 2.f;  // 1080
