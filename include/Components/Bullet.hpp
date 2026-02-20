#pragma once

struct BulletComponent {
    float   vx       = 0.f;
    float   vy       = 0.f;
    float   damage   = 10.f;
    float   speed    = 500.f;
    float   lifetime = 3.f;    // secondes avant autodestruction
    float   elapsed  = 0.f;
    bool    fromPlayer = true; // false = projectile ennemi
    bool    pierce   = false;  // traverse plusieurs ennemis (sniper)
    int     pierceLeft = 0;
};
