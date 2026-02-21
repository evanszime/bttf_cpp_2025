#pragma once
#include "../Entity.hpp"
#include "../CompContainer.hpp"
#include "../World.hpp"

class System {
protected:
    World* world = nullptr;
public:
    explicit System() = default;
    virtual ~System() = default;
    virtual void update(float dt) = 0;
    virtual void setWorld(World& w) { world = &w; }
};
