#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rngW(77);

// 8 types ennemis normaux (indices 12..19)
static const int ENEMY_TEX_BASE = 17;
static const int ENEMY_TEX_COUNT = 8;
// 7 types boss (indices 20..26)
static const int BOSS_TEX_BASE = 25;
static const int BOSS_TEX_COUNT = 7;

static void spawnEnemy(World& w, int wave, bool isBoss) {
    std::uniform_int_distribution<int> side(0,3);
    std::uniform_int_distribution<int> rx(120,1800);
    std::uniform_int_distribution<int> ry(120,960);
    int s=side(rngW);
    float px,py;
    switch(s){
        case 0: px=(float)rx(rngW); py=80;  break;
        case 1: px=(float)rx(rngW); py=1000; break;
        case 2: px=80;  py=(float)ry(rngW); break;
        default:px=1840; py=(float)ry(rngW); break;
    }

    Entity e=w.createEntity();
    PositionComponent pos; pos.x=px; pos.y=py; w.addComponent(e,pos);

    float hpBase=isBoss ? 400.f+wave*80.f : 60.f;
    HealthComponent hp; hp.hp=hp.maxHp=hpBase; w.addComponent(e,hp);

    // Sélection sprite selon type et vague
    int texIdx;
    float sz;
    if(isBoss){
        texIdx = BOSS_TEX_BASE + ((wave-1) % BOSS_TEX_COUNT);
        sz = 80.f;
    } else {
        texIdx = ENEMY_TEX_BASE + ((wave-1) % ENEMY_TEX_COUNT);
        sz = 64.f;
    }

    SpriteComponent spr;
    spr.texturePath=texIdx;
    // Adapter la taille à la texture réelle si dispo
    spr.width=sz; spr.height=sz;
    spr.left=0; spr.top=0;
    spr.scale=isBoss?1.3f:1.2f;
    spr.offsetX=-sz*spr.scale/2.f; spr.offsetY=-sz*spr.scale/2.f;
    spr.tag=isBoss?EntityTag::BOSS:EntityTag::ENEMY;
    spr.layer=isBoss?4:2;
    spr.tint=WHITE;
    w.addComponent(e,spr);

    BoxColliderComponent col; col.isCircle=true;
    col.radius=isBoss?38.f:20.f;
    col.TagsCollided={(int)EntityTag::BULLET_PL,(int)EntityTag::PLAYER};
    w.addComponent(e,col);

    // BALLES ENNEMIES : dégâts très faibles (0.1), vitesse progressive
    // Vague 1: speed=40, vague 20: speed=180
    float wf=(float)wave;
    EnemyAIComponent ai;
    if(isBoss){
        ai.speed       = 40.f + wf*3.f;         // lent
        ai.fireRate    = 1.5f;
        ai.damage      = 0.1f;                   // 0.1 dégâts
        ai.bulletSpeed = 80.f + wf*5.f;          // vitesse progressive
        ai.burstCount  = 3;
        ai.spreadAngle = 20.f;
        ai.attackRange = 500.f;
        ai.isBoss      = true;
    } else {
        ai.speed       = 30.f + wf*3.5f;         // lent au début
        ai.fireRate    = std::max(1.f, 3.f-wf*0.08f);
        ai.damage      = 0.1f;                   // 0.1 dégâts
        ai.bulletSpeed = 60.f + wf*7.f;          // vitesse progressive
        ai.burstCount  = 1;
        ai.attackRange = 400.f;
    }
    w.addComponent(e,ai);
}

void WaveSystem::update(float dt){
    if(!world||!_sm) return;
    auto& waves=world->getContainer<WaveComponent>();
    for(Entity e:waves.getEntities()){
        auto*wc=waves.get(e); if(!wc) continue;

        if(wc->state==WaveState::VICTORY){
            _sm->changeScene(SceneType::GAMEOVER); return;
        }

        // ── Entre vagues ─────────────────────────────────
        if(wc->state==WaveState::BETWEEN_WAVES){
            wc->betweenTimer-=dt;
            if(wc->betweenTimer<=0.f){
                wc->currentWave++;
                if(wc->currentWave>wc->maxWaves){ wc->state=WaveState::VICTORY; return; }
                wc->enemiesLeft=10*wc->currentWave;  // 10*N ennemis
                wc->enemiesAlive=0;
                wc->bossSpawned=false;
                wc->betweenTimer=wc->betweenMax;
                wc->spawnTimer=0.f;
                wc->state=WaveState::FIGHTING;
            }
            return;
        }

        // ── Spawn ennemis normaux ─────────────────────────
        wc->spawnTimer+=dt;
        if(wc->spawnTimer>=wc->spawnRate && wc->enemiesLeft>0){
            wc->spawnTimer=0.f;
            // Le dernier ennemi de chaque vague = boss
            bool isLast=(wc->enemiesLeft==1);
            bool spawnBoss=(isLast && !wc->bossSpawned);
            if(spawnBoss) wc->bossSpawned=true;
            spawnEnemy(*world, wc->currentWave, spawnBoss);
            wc->enemiesLeft--;
        }

        // Compter vivants
        wc->enemiesAlive=(int)world->getContainer<EnemyAIComponent>().getEntities().size();
        if(wc->enemiesLeft==0&&wc->enemiesAlive==0){
            wc->betweenTimer=wc->betweenMax;
            wc->state=WaveState::BETWEEN_WAVES;
        }
    }
}
