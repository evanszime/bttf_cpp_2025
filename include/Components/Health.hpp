#pragma once

#include <algorithm> // std::clamp, std::max
#include "../Entity.hpp"
#include "../World.hpp"
#include "../CompContainer.hpp"


struct HealthComponent
{
    float hp = 100.f;
    int deathHp = 0;
    float maxHp = 100.f;
    bool isDead = false;
};