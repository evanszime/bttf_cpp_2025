#pragma once
#include "System.hpp"
#include "../Components/Sprite.hpp"

class CollisionSystem : public System {
public:
    explicit CollisionSystem() : System() {}
    void update(float dt) override;

    // Helper: vérifie si l'entité e collide avec un tag précis
    std::pair<bool, Entity> ifCollideTag(Entity e, EntityTag tag);
};
