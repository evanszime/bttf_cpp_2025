#include "../include/Game.hpp"

void Game::loadAssets()
{
    // ============================================================
    // TEXTURES [0..37]
    // [0]  player bare   [1]  player_pistol  [2]  player_shotgun
    // [3]  player_sniper [4]  player_ak47    [5]  player_bomb
    // [6]  bullet_player [7]  bullet_enemy
    // [8]  enemy         [9]  boss
    // [10] pickup_pistol [11] pickup_shotgun [12] pickup_sniper
    // [13] pickup_rocket [14] xp_orb
    // [15] background    [16] logo
    // [17..24] enemy_00..07   [25..31] boss_00..06
    // [32] splash_activision  [33] splash_timi
    // [34] menu_bg            [35] loadbar_template
    // [36] game_map           [37] game_map_mini
    // ============================================================
    _globalComponent.addSomeSprites({
        "assets/textures/player.png",          // [0]
        "assets/textures/player_pistol.png",   // [1]
        "assets/textures/player_shotgun.png",  // [2]
        "assets/textures/player_sniper.png",   // [3]
        "assets/textures/player_ak47.png",     // [4]
        "assets/textures/player_bomb.png",     // [5]
        "assets/textures/bullet_player.png",   // [6]
        "assets/textures/bullet_enemy.png",    // [7]
        "assets/textures/enemy.png",           // [8]
        "assets/textures/boss.png",            // [9]
        "assets/textures/pickup_pistol.png",   // [10]
        "assets/textures/pickup_shotgun.png",  // [11]
        "assets/textures/pickup_sniper.png",   // [12]
        "assets/textures/pickup_rocket.png",   // [13]
        "assets/textures/xp_orb.png",          // [14]
        "assets/textures/background.png",      // [15]
        "assets/textures/logo.png",            // [16]
        "assets/textures/enemy_00.png",        // [17]
        "assets/textures/enemy_01.png",        // [18]
        "assets/textures/enemy_02.png",        // [19]
        "assets/textures/enemy_03.png",        // [20]
        "assets/textures/enemy_04.png",        // [21]
        "assets/textures/enemy_05.png",        // [22]
        "assets/textures/enemy_06.png",        // [23]
        "assets/textures/enemy_07.png",        // [24]
        "assets/textures/boss_00.png",         // [25]
        "assets/textures/boss_01.png",         // [26]
        "assets/textures/boss_02.png",         // [27]
        "assets/textures/boss_03.png",         // [28]
        "assets/textures/boss_04.png",         // [29]
        "assets/textures/boss_05.png",         // [30]
        "assets/textures/boss_06.png",         // [31]
        "assets/textures/splash_activision.png", // [32]
        "assets/textures/splash_timi.png",       // [33]
        "assets/textures/menu_bg.png",           // [34]
        "assets/textures/loadbar_template.png",  // [35]
        "assets/textures/game_map.png",          // [36]
        "assets/textures/game_map_mini.png",     // [37]
    });

    // ============================================================
    // SONS SFX courts [0..11]  — tous OGG
    // [0]  shoot_pistol    [1]  shoot_shotgun  [2]  shoot_sniper
    // [3]  shoot_ak47      [4]  shoot_rocket   [5]  enemy_die
    // [6]  pickup          [7]  reload
    // [8]  ui_click        [9]  ui_confirm
    // [10] volume_tick     [11] brightness_tick
    // ============================================================
    _globalComponent.addSomeMusics({
        "assets/sounds/shoot_pistol.ogg",    // [0]
        "assets/sounds/shoot_shotgun.ogg",   // [1]
        "assets/sounds/shoot_sniper.ogg",    // [2]
        "assets/sounds/shoot_ak47.ogg",      // [3]
        "assets/sounds/shoot_rocket.ogg",    // [4]
        "assets/sounds/enemy_die.ogg",       // [5]
        "assets/sounds/pickup.ogg",          // [6]
        "assets/sounds/reload.ogg",          // [7]
        "assets/sounds/ui_click.ogg",        // [8]
        "assets/sounds/ui_confirm.ogg",      // [9]
        "assets/sounds/volume_tick.ogg",     // [10]
        "assets/sounds/brightness_tick.ogg", // [11]
    });

    SetMasterVolume(_globalComponent.masterVolume);
}

void Game::initializeScenes()
{
    sceneManager.registerScene(SceneType::SPLASH,
        std::make_unique<SplashScene>(sceneManager, _globalComponent));
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
    sceneManager.changeScene(SceneType::SPLASH);
}
