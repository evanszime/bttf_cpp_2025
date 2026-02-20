#pragma once

struct RigidbodyComponent
{
    float vx {0.0f};
    float vy {0.0f};
    float ax {0.0f};
    float ay {0.0f};
    float mass {0.0f};
    float restitution = 0.3f;
    float fx {-1.0f};
    float fy {-1.0f};
    bool isStatic {false};
    bool useGravity {false};
    bool enableCollision = true; 
};