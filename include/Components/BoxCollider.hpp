/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-22 [WSL: Ubuntu]
** File description:
** Component.hpp
*/


#pragma once
#include <string>
#include <vector>

struct BoxColliderComponent {
    bool isCircle = false;
    float width = 0.f;
    float height = 0.f;
    float radius = 0.f;
    bool isTrigger = false;
    std::vector<int> PreviousCollisions;
    std::vector<int> EntityCollided;
    std::vector<int> layersCollided;
    std::vector<int> TagsCollided;
};
