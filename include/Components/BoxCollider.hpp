#pragma once
#include <vector>
struct BoxColliderComponent {
    bool isCircle=false; float width=0,height=0,radius=0; bool isTrigger=false;
    std::vector<int> PreviousCollisions,EntityCollided,layersCollided,TagsCollided;
};
