#pragma once
#include <vector>
#include <string>

// État global de la vague
enum class WaveState {
    SHOPPING,    // entre deux vagues - shop (simplifié: juste un timer)
    FIGHTING,    // vague en cours
    WAVE_CLEAR,  // vague terminée, transition
    GAME_OVER,
    VICTORY      // vague 20 terminée
};

struct WaveComponent {
    int       currentWave  = 1;
    int       maxWaves     = 20;
    WaveState state        = WaveState::FIGHTING;

    // Spawn
    int       enemiesLeft  = 0;   // ennemis restants à spawner
    int       enemiesAlive = 0;   // ennemis actuellement vivants
    float     spawnTimer   = 0.f;
    float     spawnRate    = 1.5f; // secondes entre spawns
    float     waveTimer    = 0.f;  // durée de la vague en cours
    float     waveDuration = 30.f; // durée max par vague (s)

    // Transition
    float     transitionTimer = 0.f;
    float     transitionDelay = 3.f; // secondes entre vagues

    // Difficulté scalée
    int   baseEnemiesPerWave = 5;
    float hpMultiplier       = 1.f;
    float speedMultiplier    = 1.f;
};
