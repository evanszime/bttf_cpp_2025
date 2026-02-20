#pragma once
#include <string>
#include <raylib.h>

// Tags entité - identifie le type pour collisions et logique
enum class EntityTag {
    NONE      = 0,
    PLAYER    = 1,
    ENEMY     = 2,
    BOSS      = 3,
    BULLET_PL = 4,  // projectile du joueur
    BULLET_EN = 5,  // projectile ennemi
    PICKUP    = 6,  // arme au sol / item ramassable
    WALL      = 7,
    XP_ORB    = 8,  // orbe d'XP (Brotato: matériaux verts)
    UI        = 9,
};

struct SpriteComponent {
    // === TEXTURE ===
    // texturePath = index dans GlobalComponent::_allSprites
    // Mets ici l'index correspondant à ta texture chargée via addSprite()
    // ASSET: voir assets/textures/ - tu dois charger ta texture dans Game::loadsprites()
    int   texturePath = 0;   // << INDEX_ASSET (0 = première texture chargée)
    float left   = 0.f;
    float top    = 0.f;
    float width  = 32.f;
    float height = 32.f;

    // === RENDU ===
    float     scale    = 1.f;
    float     rotation = 0.f;   // degrés
    Color     tint     = WHITE;
    int       layer    = 0;     // ordre de rendu (0=fond, 10=UI)
    EntityTag tag      = EntityTag::NONE;
    bool      visible  = true;

    // Offset d'affichage par rapport à PositionComponent
    float offsetX = 0.f;
    float offsetY = 0.f;
};
