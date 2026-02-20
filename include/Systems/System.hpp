/*
** EPITECH PROJECT, 2025
** G-CPP-500-COT-5-1-rtype-22 [WSL: Ubuntu]
** File description:
** System.hpp
*/

#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "../Entity.hpp"
#include "../CompContainer.hpp"
#include "../World.hpp"
#include <functional>

class System {
protected:
    World *world;

public:
    explicit System() : world(nullptr) {}
    virtual ~System() = default;

    virtual void update(float dt) = 0;
    virtual void setWorld(World& wo) {
        world = &wo;
    }
    // System operation=(System s){
    //     return s;
    // }
};
