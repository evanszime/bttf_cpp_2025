#include "../include/Game.hpp"

void Game::loadAssets()
{
    // ================================================================
    // INDEX TEXTURES (ne pas changer l'ordre !)
    // [0]  player.png          [1]  enemy.png (défaut)   [2]  boss.png (défaut)
    // [3]  bullet_player.png   [4]  bullet_enemy.png     [5]  pickup_pistol.png
    // [6]  pickup_shotgun.png  [7]  pickup_sniper.png    [8]  pickup_rocket.png
    // [9]  xp_orb.png          [10] background.png       [11] logo.png
    // [12-19] enemy_00..07.png (8 types ennemis Brotato)
    // [20-26] boss_00..06.png  (7 types boss Brotato)
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
        // Ennemis Brotato (8 types)
        "assets/textures/enemy_00.png",       // [12]
        "assets/textures/enemy_01.png",       // [13]
        "assets/textures/enemy_02.png",       // [14]
        "assets/textures/enemy_03.png",       // [15]
        "assets/textures/enemy_04.png",       // [16]
        "assets/textures/enemy_05.png",       // [17]
        "assets/textures/enemy_06.png",       // [18]
        "assets/textures/enemy_07.png",       // [19]
        // Boss Brotato (7 types)
        "assets/textures/boss_00.png",        // [20]
        "assets/textures/boss_01.png",        // [21]
        "assets/textures/boss_02.png",        // [22]
        "assets/textures/boss_03.png",        // [23]
        "assets/textures/boss_04.png",        // [24]
        "assets/textures/boss_05.png",        // [25]
        "assets/textures/boss_06.png",        // [26]
        // Player sprite sheet (2 frames animation)
        "assets/textures/player_sheet.png",   // [27]
    });

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
    sceneManager.registerScene(SceneType::INFO,
        std::make_unique<InfoScene>(sceneManager));
    sceneManager.registerScene(SceneType::GAME,
        std::make_unique<GameScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::GAMEOVER,
        std::make_unique<GameOverScene>(sceneManager, false));
}
