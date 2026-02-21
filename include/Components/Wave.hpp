#pragma once
enum class WaveState { FIGHTING, BETWEEN_WAVES, BOSS_WAVE, VICTORY };

struct WaveComponent {
    int       currentWave     = 1;
    int       maxWaves        = 20;
    int       enemiesLeft     = 0;    // ennemis restant à spawner
    int       enemiesAlive    = 0;    // ennemis actuellement vivants
    int       baseEnemies     = 10;   // vague 1 = 10, vague N = 10*N
    float     spawnRate       = 1.2f;
    float     spawnTimer      = 0.f;
    float     betweenTimer    = 4.f;
    float     betweenMax      = 4.f;
    WaveState state           = WaveState::FIGHTING;
    bool      bossSpawned     = false;
};
