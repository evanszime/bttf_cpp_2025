#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "Components/Animation.hpp"

class GlobalComponent {
public:
    // TEXTURES - charger dans Game.cpp via addSprite() dans cet ordre:
    //  [0] player.png        [1] enemy.png         [2] boss.png
    //  [3] bullet_player.png [4] bullet_enemy.png  [5] pickup_pistol.png
    //  [6] pickup_shotgun.png[7] pickup_sniper.png  [8] pickup_rocket.png
    //  [9] xp_orb.png       [10] background.png   [11] logo.png
    std::vector<Texture2D> _allSprites;

    // SONS - charger via addMusic():
    //  [0] shoot_pistol  [1] shoot_shotgun [2] shoot_rocket
    //  [3] enemy_die     [4] pickup        [5] reload
    //  [6] music_menu    [7] music_game
    std::vector<Sound> _allMusics;

    // ANIMATIONS: animSetKey -> animName -> AnimationSequence
    std::unordered_map<std::string,
        std::unordered_map<std::string, AnimationSequence>> _allAnimations;

    // SETTINGS persistants
    float masterVolume = 0.5f;
    bool  isFullscreen = false;

    void addSprite(const std::string& path);
    void addSomeSprites(const std::vector<std::string>& paths);
    void addMusic(const std::string& path);
    void addSomeMusics(const std::vector<std::string>& paths);
};

extern GlobalComponent _globalComponent;
