#pragma once
#include <vector>
#include <raylib.h>

// Map 4K générée par code (3840x2160)
// Roches dessinées à des positions précises — rectangles de collision alignés
static constexpr int MAP_WORLD_W = 3840;
static constexpr int MAP_WORLD_H = 2160;

// ═══════════════════════════════════════════════════════════════
// RECTANGLES DE COLLISION DES ROCHES
// Chaque rectangle correspond exactement à une roche dessinée
// sur game_map.png. Marges incluses pour un ressenti naturel.
// ═══════════════════════════════════════════════════════════════
static const std::vector<Rectangle> MAP_OBSTACLES = {
    {768.f,  233.f,  264.f, 174.f},  // roche angulaire  (900, 320)
    {2414.f, 332.f,  173.f, 116.f},  // roche arrondie   (2500, 390)
    {2952.f, 828.f,  297.f, 145.f},  // roche plate      (3100, 900)
    {551.f,  1035.f, 198.f, 130.f},  // roche angulaire  (650, 1100)
    {1781.f, 978.f,  239.f, 145.f},  // roche arrondie   (1900, 1050)
    {3126.f, 1250.f, 148.f, 101.f},  // roche angulaire  (3200, 1300)
    {373.f,  1621.f, 255.f, 159.f},  // roche plate      (500, 1700)
    {2010.f, 1742.f, 181.f, 116.f},  // roche arrondie   (2100, 1800)
    {1230.f, 553.f,  140.f,  94.f},  // roche angulaire  (1300, 600)
    {2693.f, 1539.f, 214.f, 123.f},  // roche plate      (2800, 1600)
    {693.f,  661.f,  115.f,  79.f},  // roche arrondie   (750, 700)
};

static constexpr float SPAWN_CENTER_X = MAP_WORLD_W / 2.f;  // 1920
static constexpr float SPAWN_CENTER_Y = MAP_WORLD_H / 2.f;  // 1080
