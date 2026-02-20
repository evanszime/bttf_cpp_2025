#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 waveRng(12345);

// Positions de spawn aléatoires sur le bord de l'arène
static std::pair<float,float> randomSpawnPos()
{
    std::uniform_int_distribution<int> side(0, 3);
    std::uniform_real_distribution<float> posX(0.f, 1920.f);
    std::uniform_real_distribution<float> posY(0.f, 1080.f);

    int s = side(waveRng);
    switch(s) {
        case 0: return {posX(waveRng), -30.f};     // top
        case 1: return {posX(waveRng), 1110.f};    // bottom
        case 2: return {-30.f,  posY(waveRng)};    // left
        default:return {1950.f, posY(waveRng)};    // right
    }
}

void WaveSystem::update(float dt)
{
    if (!world || !_sm) return;

    auto& waveComps = world->getContainer<WaveComponent>();
    if (waveComps.getEntities().empty()) return;

    Entity we  = waveComps.getEntities()[0];
    auto*  wc  = waveComps.get(we);
    if (!wc) return;

    switch (wc->state) {

    case WaveState::FIGHTING: {
        wc->waveTimer += dt;

        // Spawn d'ennemis
        wc->spawnTimer += dt;
        if (wc->spawnTimer >= wc->spawnRate && wc->enemiesLeft > 0) {
            wc->spawnTimer = 0.f;
            wc->enemiesLeft--;
            wc->enemiesAlive++;

            auto [sx, sy] = randomSpawnPos();

            // Boss à la vague 10 et 20
            bool isBoss = (wc->currentWave % 10 == 0 && wc->enemiesLeft == 0);

            // EnemySystem::spawnEnemy via pointeur world
            // On instancie un EnemySystem temporaire
            EnemySystem es;
            es.setWorld(*world);
            es.spawnEnemy(sx, sy, wc->currentWave, isBoss);
        }

        // Vague terminée: tous les ennemis spawned et morts
        bool allDead = (wc->enemiesLeft == 0 && wc->enemiesAlive == 0);
        bool timeout = (wc->waveTimer >= wc->waveDuration);

        if (allDead || timeout) {
            wc->state = WaveState::WAVE_CLEAR;
            wc->transitionTimer = 0.f;

            // Collecter les XP orbs restants
            auto& sprites   = world->getContainer<SpriteComponent>();
            auto& playersC  = world->getContainer<PlayerComponent>();
            auto& positions = world->getContainer<PositionComponent>();

            // Trouver le joueur
            Entity player{0};
            for (Entity e : sprites.getEntities()) {
                auto* s = sprites.get(e);
                if (s && s->tag == EntityTag::PLAYER) { player = e; break; }
            }
            auto* pl = player != Entity{0} ? playersC.get(player) : nullptr;

            // Auto-collect XP orbs
            std::vector<Entity> toRm;
            for (Entity e : sprites.getEntities()) {
                auto* s = sprites.get(e);
                if (s && s->tag == EntityTag::XP_ORB) {
                    if (pl) pl->materials++;
                    toRm.push_back(e);
                }
            }
            for (Entity e : toRm) world->removeEntity(e);
        }
        break;
    }

    case WaveState::WAVE_CLEAR: {
        wc->transitionTimer += dt;
        if (wc->transitionTimer >= wc->transitionDelay) {
            wc->currentWave++;
            if (wc->currentWave > wc->maxWaves) {
                wc->state = WaveState::VICTORY;
                _sm->changeScene(SceneType::VICTORY);
            } else {
                // Préparer vague suivante
                wc->state       = WaveState::FIGHTING;
                wc->waveTimer   = 0.f;
                wc->spawnTimer  = 0.f;
                wc->enemiesAlive= 0;

                // Nombre d'ennemis = base + progression
                int base = wc->baseEnemiesPerWave + (wc->currentWave - 1) * 2;
                wc->enemiesLeft = base;
                wc->spawnRate   = std::max(0.5f, 1.5f - (wc->currentWave - 1) * 0.05f);
                wc->waveDuration= 30.f + wc->currentWave * 2.f;
            }
        }
        break;
    }

    case WaveState::GAME_OVER:
        _sm->changeScene(SceneType::GAME_OVER);
        break;

    default: break;
    }
}
