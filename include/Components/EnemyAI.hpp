#pragma once
#include <cstddef>
#include "Entity.hpp"

// Type de boss (vague 1..6, boss 6 = final)
enum class BossType {
    NONE    = 0,
    SHOTGUN = 1,   // Vague 1 : balles shotgun violettes
    SNIPER  = 2,   // Vague 2 : balles sniper blanches
    BOMB    = 3,   // Vague 3 : bombes jaunes
    ROCKET  = 4,   // Vague 4 : rockets rouge vif
    SHOCKWAVE = 5, // Vague 5 : bombes + onde de choc + heal sbires
    FINAL   = 6,   // Vague 6 : tout + phases de rage + élève sbires
};

// Couleur de balle selon boss
struct BulletStyle {
    Color   tint    = {255,80,0,255};
    float   width   = 10.f;
    float   height  = 10.f;
    float   radius  = 5.f;
};

struct EnemyAIComponent {
    float speed          = 80.f;
    float detectionRange = 1200.f;
    float attackRange    = 500.f;
    float fireRate       = 2.f;
    float lastFireTime   = 0.f;
    float bulletSpeed    = 250.f;
    float damage         = 10.f;
    int   burstCount     = 1;
    float spreadAngle    = 0.f;

    bool     isBoss     = false;
    BossType bossType   = BossType::NONE;
    int      bossPhase  = 0;   // 0=normal, 1=80%, 2=60%, 3=40%, 4=20%

    // Boss 5 : heal sbires
    float healTimer    = 0.f;
    float healInterval = 8.f;
    int   healCount    = 0;

    // Boss 6 (Final) : élève les sbires
    float upgradeTimer    = 0.f;
    float upgradeInterval = 15.f;
    int   upgradeCount    = 0;

    // Style des balles ennemies
    BulletStyle bulletStyle;

    Entity targetEntity = Entity(0);
    bool   hasTarget    = false;
    float  stateTimer   = 0.f;
};
