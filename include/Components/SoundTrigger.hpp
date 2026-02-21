#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
struct Sound_t {
    int   soundPath = 0;
    float volume    = 1.f;
    int   countPlay = 1;
    bool  loop      = false;
    bool  isActive  = false;
};
struct SoundTriggerComponent {
    std::unordered_map<std::string, Sound_t> sounds;
};
