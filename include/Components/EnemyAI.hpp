#pragma once
#include <cstddef>
#include "Entity.hpp"
struct EnemyAIComponent {
    float speed        = 80.f;
    float detectionRange = 600.f;
    float attackRange  = 280.f;
    float fireRate     = 2.f;
    float lastFireTime = 0.f;
    float bulletSpeed  = 250.f;
    float damage       = 10.f;
    int   burstCount   = 1;
    float spreadAngle  = 0.f;
    bool  isBoss       = false;
    int   bossPhase    = 0;
    Entity targetEntity = Entity(0);
    bool   hasTarget   = false;
    float  stateTimer  = 0.f;
};
