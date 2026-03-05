#include "../include/Game.hpp"

void Game::loadAssets()
{
    // ================================================================
    // INDEX TEXTURES — NE PAS CHANGER L'ORDRE
    // ── Player sheets (0-5) ─────────────────────────────────────────
    // [0]  player.png          288x256 — sheet sans arme (idle/swap)
    // [1]  player_pistol.png   288x256 — player + pistol
    // [2]  player_shotgun.png  288x256 — player + shotgun
    // [3]  player_sniper.png   288x256 — player + sniper
    // [4]  player_ak47.png     288x256 — player + ak47
    // [5]  player_bomb.png     288x256 — player + bomb
    // ── Bullet / Enemies (6-12) ──────────────────────────────────────
    // [6]  bullet_player.png   [7]  bullet_enemy.png
    // [8]  enemy.png  (défaut) [9]  boss.png (défaut)
    // ── Pickups (10-14) ──────────────────────────────────────────────
    // [10] pickup_pistol  [11] pickup_shotgun  [12] pickup_sniper
    // [13] pickup_rocket  [14] xp_orb
    // ── UI / Décor (15-16) ───────────────────────────────────────────
    // [15] background.png   [16] logo.png
    // ── Ennemis Brotato (17-24) ──────────────────────────────────────
    // [17..24] enemy_00..07
    // ── Boss Brotato (25-31) ─────────────────────────────────────────
    // [25..31] boss_00..06
    // ── Cinématique & UI (32-36) ─────────────────────────────────────
    // [32] splash_activision.png  [33] splash_timi.png
    // [34] menu_bg.png
    // [35] loadbar_template.png
    // ================================================================
    _globalComponent.addSomeSprites({
        // Player sheets
        "assets/textures/player.png",          // [0]  bare
        "assets/textures/player_pistol.png",   // [1]
        "assets/textures/player_shotgun.png",  // [2]
        "assets/textures/player_sniper.png",   // [3]
        "assets/textures/player_ak47.png",     // [4]
        "assets/textures/player_bomb.png",     // [5]
        // Bullets
        "assets/textures/bullet_player.png",   // [6]
        "assets/textures/bullet_enemy.png",    // [7]
        // Enemies defaults
        "assets/textures/enemy.png",           // [8]
        "assets/textures/boss.png",            // [9]
        // Pickups
        "assets/textures/pickup_pistol.png",   // [10]
        "assets/textures/pickup_shotgun.png",  // [11]
        "assets/textures/pickup_sniper.png",   // [12]
        "assets/textures/pickup_rocket.png",   // [13]
        "assets/textures/xp_orb.png",          // [14]
        // UI
        "assets/textures/background.png",      // [15]
        "assets/textures/logo.png",            // [16]
        // Ennemis Brotato
        "assets/textures/enemy_00.png",        // [17]
        "assets/textures/enemy_01.png",        // [18]
        "assets/textures/enemy_02.png",        // [19]
        "assets/textures/enemy_03.png",        // [20]
        "assets/textures/enemy_04.png",        // [21]
        "assets/textures/enemy_05.png",        // [22]
        "assets/textures/enemy_06.png",        // [23]
        "assets/textures/enemy_07.png",        // [24]
        // Boss Brotato
        "assets/textures/boss_00.png",         // [25]
        "assets/textures/boss_01.png",         // [26]
        "assets/textures/boss_02.png",         // [27]
        "assets/textures/boss_03.png",         // [28]
        "assets/textures/boss_04.png",         // [29]
        "assets/textures/boss_05.png",         // [30]
        "assets/textures/boss_06.png",         // [31]
        // Cinématique
        "assets/textures/splash_activision.png", // [32]
        "assets/textures/splash_timi.png",       // [33]
        "assets/textures/menu_bg.png",           // [34]
        "assets/textures/loadbar_template.png",  // [35]
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
    // Démarrer par le splash
    sceneManager.changeScene(SceneType::SPLASH);
}
