#pragma once

// Stats Brotato-style du joueur
struct PlayerComponent {
    // === STATS COMBAT ===
    float speed      = 200.f;   // vitesse de déplacement
    float damage     = 1.f;     // multiplicateur de dégâts
    int   armor      = 0;       // réduction de dégâts flat

    // === BROTATO STATS ===
    int   materials  = 0;       // matériaux verts collectés
    int   xp         = 0;
    int   level      = 1;
    int   wave       = 1;       // vague courante (max 20)
    int   kills      = 0;

    // === ÉTAT ===
    float invincTimer = 0.f;    // secondes d'invincibilité après un hit
    bool  isInvincible = false;

    // === MOUVEMENT ===
    float lookAngle  = 0.f;    // angle vers la souris (degrés)

    // === RELOAD (touche R) ===
    bool  wantsReload = false;
};
