#pragma once
struct PlayerComponent {
    // Stats
    float speed         = 400.f;
    float damage        = 1.f;
    int   armor         = 0;
    int   materials     = 0;
    int   xp            = 0;
    int   level         = 1;
    int   kills         = 0;

    // Vie : 200 HP
    // (HealthComponent gère les HP réels)

    // Bouclier magnétique : 360 HP (0 = pas de bouclier)
    float shieldHp      = 0.f;
    float shieldMax     = 360.f;
    bool  hasShield     = false;

    // Invincibilité temporaire après dégâts
    float invincTimer   = 0.f;
    bool  isInvincible  = false;
};
