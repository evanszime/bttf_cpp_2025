#include "../include/Game.hpp"

void Game::loadAssets()
{
    // ================================================================
    // TEXTURES - Index = ordre de chargement
    // Télécharge les assets depuis Brotato Legacy Demo ou crée tes propres sprites
    // Site: https://thomasgvd.itch.io/brotato-legacy-demo
    // ================================================================
    _globalComponent.addSomeSprites({
        // [0] Joueur
        "assets/textures/player.png",       // << ASSET REQUIS
        // [1] Ennemi de base
        "assets/textures/enemy.png",        // << ASSET REQUIS
        // [2] Boss
        "assets/textures/boss.png",         // << ASSET REQUIS
        // [3] Projectile joueur
        "assets/textures/bullet_player.png",// << ASSET REQUIS
        // [4] Projectile ennemi
        "assets/textures/bullet_enemy.png", // << ASSET REQUIS
        // [5] Pickup pistol (non utilisé comme pickup par défaut mais utile)
        "assets/textures/pickup_pistol.png",// << ASSET OPTIONNEL
        // [6] Pickup shotgun
        "assets/textures/pickup_shotgun.png",// << ASSET OPTIONNEL
        // [7] Pickup sniper
        "assets/textures/pickup_sniper.png",// << ASSET REQUIS (spawn en jeu)
        // [8] Pickup rocket
        "assets/textures/pickup_rocket.png",// << ASSET REQUIS (spawn en jeu)
        // [9] Orbe XP (matériaux verts Brotato)
        "assets/textures/xp_orb.png",       // << ASSET REQUIS
        // [10] Background
        "assets/textures/background.png",   // << ASSET REQUIS
        // [11] Logo/titre
        "assets/textures/logo.png",         // << ASSET OPTIONNEL
    });

    // ================================================================
    // SONS
    // ================================================================
    _globalComponent.addSomeMusics({
        // [0] Tir pistol
        "assets/sounds/shoot_pistol.wav",   // << ASSET REQUIS
        // [1] Tir shotgun
        "assets/sounds/shoot_shotgun.wav",  // << ASSET REQUIS
        // [2] Tir rocket
        "assets/sounds/shoot_rocket.wav",   // << ASSET OPTIONNEL
        // [3] Mort ennemi
        "assets/sounds/enemy_die.wav",      // << ASSET OPTIONNEL
        // [4] Pickup item
        "assets/sounds/pickup.wav",         // << ASSET OPTIONNEL
        // [5] Reload
        "assets/sounds/reload.wav",         // << ASSET OPTIONNEL
        // [6] Musique menu
        "assets/sounds/music_menu.ogg",     // << ASSET OPTIONNEL
        // [7] Musique jeu
        "assets/sounds/music_game.ogg",     // << ASSET OPTIONNEL
    });
}

void Game::initializeScenes()
{
    sceneManager.registerScene(SceneType::MENU,
        std::make_unique<MenuScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::GAME,
        std::make_unique<GameScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::SETTINGS,
        std::make_unique<SettingsScene>(sceneManager, _globalComponent));
    sceneManager.registerScene(SceneType::GAME_OVER,
        std::make_unique<GameOverScene>(sceneManager));
    sceneManager.registerScene(SceneType::VICTORY,
        std::make_unique<VictoryScene>(sceneManager));
}
