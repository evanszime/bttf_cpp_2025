#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "Components/Animation.hpp"

class GlobalComponent {
public:
    // Toutes les textures du jeu (indexées par ordre de chargement)
    // ASSET: charger via addSprite("assets/textures/xxx.png")
    // Index des textures attendues dans Zimex:
    //   0 = player        << assets/textures/player.png
    //   1 = enemy         << assets/textures/enemy.png
    //   2 = boss          << assets/textures/boss.png
    //   3 = bullet_player << assets/textures/bullet_player.png
    //   4 = bullet_enemy  << assets/textures/bullet_enemy.png
    //   5 = pickup_pistol << assets/textures/pickup_pistol.png
    //   6 = pickup_shotgun<< assets/textures/pickup_shotgun.png
    //   7 = pickup_sniper << assets/textures/pickup_sniper.png
    //   8 = pickup_rocket << assets/textures/pickup_rocket.png
    //   9 = xp_orb        << assets/textures/xp_orb.png
    //  10 = background    << assets/textures/background.png
    //  11 = logo/title    << assets/textures/logo.png
    std::vector<Texture2D> _allSprites;

    // Sons indexés:
    //   0 = shoot_pistol  << assets/sounds/shoot_pistol.wav
    //   1 = shoot_shotgun << assets/sounds/shoot_shotgun.wav
    //   2 = shoot_rocket  << assets/sounds/shoot_rocket.wav
    //   3 = enemy_die     << assets/sounds/enemy_die.wav
    //   4 = pickup        << assets/sounds/pickup.wav
    //   5 = reload        << assets/sounds/reload.wav
    //   6 = music_menu    << assets/sounds/music_menu.ogg
    //   7 = music_game    << assets/sounds/music_game.ogg
    std::vector<Sound> _allMusics;

    // Animations par nom de set puis par nom d'anim
    std::unordered_map<std::string,
        std::unordered_map<std::string, AnimationSequence>> _allAnimations;

    // Settings persistants
    float masterVolume   = 0.5f;
    bool  isFullscreen   = false;

    void addSprite(const std::string& path);
    void addSomeSprites(const std::vector<std::string>& paths);
    void addMusic(const std::string& path);
    void addSomeMusics(const std::vector<std::string>& paths);
};

extern GlobalComponent _globalComponent;
