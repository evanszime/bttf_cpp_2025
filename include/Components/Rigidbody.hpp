#pragma once
struct RigidbodyComponent {
    float vx=0,vy=0,ax=0,ay=0,mass=1.f,restitution=0.3f,fx=0,fy=0;
    bool isStatic=false,useGravity=false,enableCollision=true;
};
