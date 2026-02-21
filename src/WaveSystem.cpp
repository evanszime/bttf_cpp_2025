#include "../include/Engine.hpp"
#include <cmath>
#include <cstdlib>
#include <random>

static std::mt19937 rngW(77);

static void spawnEnemy(World& w, int wave, bool isBoss){
    // Position sur les bords de l'arène
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

    // HP : 60 pour ennemi normal, boss = 400 + wave*80
    float hpBase = isBoss ? 400.f+wave*80.f : 60.f;
    HealthComponent hp; hp.hp=hp.maxHp=hpBase; w.addComponent(e,hp);

    SpriteComponent spr;
    spr.texturePath=isBoss?2:1;
    float sz=isBoss?64.f:32.f;
    spr.width=sz; spr.height=sz;
    spr.left=0; spr.top=0; spr.scale=isBoss?1.5f:1.4f;
    spr.offsetX=-sz*spr.scale/2.f; spr.offsetY=-sz*spr.scale/2.f;
    spr.tag=isBoss?EntityTag::BOSS:EntityTag::ENEMY;
    spr.layer=isBoss?4:2;
    spr.tint=isBoss?Color{220,30,30,255}:WHITE;
    w.addComponent(e,spr);

    BoxColliderComponent col; col.isCircle=true;
    col.radius=isBoss?36.f:18.f;
    col.TagsCollided={(int)EntityTag::BULLET_PL,(int)EntityTag::PLAYER};
    w.addComponent(e,col);

    EnemyAIComponent ai;
    // Vitesse et dégâts progressifs
    float wf=(float)wave;
    if(isBoss){
        ai.speed      =55.f+wf*2.f;
        ai.fireRate   =1.2f;
        ai.damage     =20.f+wf*1.5f;
        ai.bulletSpeed=260.f+wf*5.f;
        ai.burstCount =3;
        ai.spreadAngle=20.f;
        ai.attackRange=450.f;
        ai.isBoss     =true;
    } else {
        ai.speed      =65.f+wf*4.f;
        ai.fireRate   =std::max(0.8f, 2.5f-wf*0.08f);
        ai.damage     =8.f+wf*0.5f;
        ai.bulletSpeed=200.f+wf*8.f;
        ai.burstCount =1;
        ai.attackRange=350.f;
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
        if(wc->state==WaveState::BETWEEN_WAVES||wc->state==WaveState::BOSS_WAVE){
            wc->betweenTimer-=dt;
            if(wc->betweenTimer<=0.f){
                wc->currentWave++;
                if(wc->currentWave>wc->maxWaves){ wc->state=WaveState::VICTORY; return; }
                // Vague N = 10*N ennemis
                wc->enemiesLeft=10*wc->currentWave;
                wc->enemiesAlive=0;
                wc->bossSpawned=false;
                wc->betweenTimer=wc->betweenMax;
                wc->spawnTimer=0.f;
                wc->state=WaveState::FIGHTING;
            }
            return;
        }

        // ── Spawn ennemis ─────────────────────────────────
        wc->spawnTimer+=dt;
        if(wc->spawnTimer>=wc->spawnRate&&wc->enemiesLeft>0){
            wc->spawnTimer=0.f;
            // Boss : spawne SEULEMENT à la fin de la vague (dernier ennemi), vagues 3,6,9...
            bool isBossWave=(wc->currentWave%3==0);
            bool spawnBoss=(isBossWave && wc->enemiesLeft==1 && !wc->bossSpawned);
            if(spawnBoss) wc->bossSpawned=true;
            spawnEnemy(*world, wc->currentWave, spawnBoss);
            wc->enemiesLeft--;
        }

        // ── Compter ennemis vivants ───────────────────────
        wc->enemiesAlive=(int)world->getContainer<EnemyAIComponent>().getEntities().size();
        if(wc->enemiesLeft==0&&wc->enemiesAlive==0){
            wc->betweenTimer=wc->betweenMax;
            wc->state=WaveState::BETWEEN_WAVES;
        }
    }
}
