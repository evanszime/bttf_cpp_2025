#pragma once
#include <raylib.h>

enum class EntityTag {
    NONE        = 0,
    PLAYER      = 1,
    ENEMY       = 2,
    BOSS        = 3,
    BULLET_PL   = 4,
    BULLET_EN   = 5,
    PICKUP      = 6,   // arme
    WALL        = 7,
    XP_ORB      = 8,
    UI          = 9,
    PICKUP_AMMO = 10,  // recharge munitions
    PICKUP_HP   = 11,  // soin +50 HP
    PICKUP_SHIELD = 12,// bouclier magnétique 360 HP
    EXPLOSION   = 13,  // effet visuel temporaire
};

struct SpriteComponent {
    int   texturePath = 0;
    float left    = 0.f;
    float top     = 0.f;
    float width   = 32.f;
    float height  = 32.f;
    float scale   = 1.f;
    float rotation = 0.f;
    Color tint    = WHITE;
    int   layer   = 0;
    EntityTag tag = EntityTag::NONE;
    bool  visible = true;
    float offsetX = 0.f;
    float offsetY = 0.f;
};
