#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include <unordered_map>
struct AnimationFrame { Rectangle rect; };
struct AnimationSequence {
    std::vector<AnimationFrame> frames;
    float fps  = 8.f;
    bool  loop = true;
};
struct AnimationComponent {
    std::string animations    = "";   // clé dans _allAnimations
    std::string currentAnim   = "idle";
    std::size_t currentFrame  = 0;
    float       elapsed       = 0.f;
    float       speed         = 1.f;
    bool        playing       = true;
    bool        autoDestroy   = false;
};
