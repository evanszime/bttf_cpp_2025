#pragma once
#include "System.hpp"
#include "../Components/Sprite.hpp"
#include "../Components/BoxCollider.hpp"

class CollisionSystem : public System {
public:
    explicit CollisionSystem() = default;
    std::pair<bool, Entity> ifCollideTag(Entity e, int tag);
    void update(float dt) override;
};
