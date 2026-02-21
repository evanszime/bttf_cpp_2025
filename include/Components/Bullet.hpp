#pragma once
#include <string>

enum class BulletEffect {
    NONE,
    EXPLOSION,   // BombLauncher : AoE 120px dégâts splash
    BURST,       // AK47 : pas d'effet spécial, juste rafale
    PIERCE,      // Sniper : traverse ennemis
};

struct BulletComponent {
    float vx       = 0.f;
    float vy       = 0.f;
    float speed    = 500.f;
    float damage   = 20.f;
    float range    = 800.f;
    float traveled = 0.f;
    bool  isEnemy  = false;

    // Effets spéciaux
    BulletEffect effect     = BulletEffect::NONE;
    int          pierceLeft = 0;    // Sniper
    float        aoeRadius  = 0.f;  // BombLauncher
    float        aoeDamage  = 0.f;
};
