#include "../include/Game.hpp"

void Game::loadAssets()
{
    // ================================================================
    // TEXTURES  (index = ordre de chargement - NE PAS CHANGER L'ORDRE)
    // Placer les fichiers dans assets/textures/
    // Si un fichier manque -> placeholder couleur automatique
    // ================================================================
    _globalComponent.addSomeSprites({
        "assets/textures/player.png",         // [0]
        "assets/textures/enemy.png",          // [1]
        "assets/textures/boss.png",           // [2]
        "assets/textures/bullet_player.png",  // [3]
        "assets/textures/bullet_enemy.png",   // [4]
        "assets/textures/pickup_pistol.png",  // [5]
        "assets/textures/pickup_shotgun.png", // [6]
        "assets/textures/pickup_sniper.png",  // [7]
        "assets/textures/pickup_rocket.png",  // [8]
        "assets/textures/xp_orb.png",         // [9]
        "assets/textures/background.png",     // [10]
        "assets/textures/logo.png",           // [11]
    });

    // SONS  - placer dans assets/sounds/
    _globalComponent.addSomeMusics({
        "assets/sounds/shoot_pistol.wav",   // [0]
        "assets/sounds/shoot_shotgun.wav",  // [1]
        "assets/sounds/shoot_rocket.wav",   // [2]
        "assets/sounds/enemy_die.wav",      // [3]
        "assets/sounds/pickup.wav",         // [4]
        "assets/sounds/reload.wav",         // [5]
        "assets/sounds/music_menu.ogg",     // [6]
        "assets/sounds/music_game.ogg",     // [7]
    });

    SetMasterVolume(_globalComponent.masterVolume);
}

void Game::initializeScenes()
{
    sceneManager.registerScene(SceneType::MENU,
        std::make_unique<MenuScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::SETTINGS,
        std::make_unique<SettingsScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::GAME,
        std::make_unique<GameScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::GAMEOVER,
        std::make_unique<GameOverScene>(sceneManager, false));
}
