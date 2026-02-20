#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>

struct AnimationFrame {
    Rectangle rect; // Rectangle Raylib natif (x, y, width, height dans le spritesheet)
};

struct AnimationSequence {
    std::vector<AnimationFrame> frames;
    float fps  = 8.f;
    bool  loop = true;
};

struct AnimationComponent {
    std::string animations;     // clé dans _globalComponent._allAnimations
    std::string currentAnim = "idle";
    std::size_t currentFrame = 0;
    float       elapsed      = 0.f;
    float       speed        = 1.f;
    bool        playing      = true;
    bool        autoDestroy  = false; // détruire l'entité en fin d'anim (ex: explosion)
};
