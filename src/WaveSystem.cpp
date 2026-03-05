#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rngW(77);

// Indices textures ennemis/boss
static const int ENEMY_TEX_BASE  = 17;  // enemy_00..07
static const int ENEMY_TEX_COUNT = 8;
static const int BOSS_TEX_BASE   = 25;  // boss_00..05
static const int BOSS_TEX_COUNT  = 6;

// Trouver le joueur
static Entity findPlayer(World& w) {
    for (Entity e : w.getContainer<PlayerComponent>().getEntities()) return e;
    return Entity(0);
}

// ── Bullet style par BossType ──────────────────────────────────
static BulletStyle bossStyle(BossType t) {
    switch(t) {
    case BossType::SHOTGUN:  return {{160,0,220,255},  10,10, 5};  // violet
    case BossType::SNIPER:   return {{220,240,255,255}, 16, 6, 5}; // blanc allongé
    case BossType::BOMB:     return {{255,230,0,255},   16,16, 8}; // jaune gros
    case BossType::ROCKET:   return {{255,20,20,255},   14, 8, 6}; // rouge vif
    case BossType::SHOCKWAVE:return {{255,230,0,255},   18,18, 9}; // jaune (même bomb)
    case BossType::FINAL:    return {{255,100,0,255},   12,12, 6}; // orange default
    default:                 return {{255,80,0,255},    10,10, 5};
    }
}

// ── Spawn boss bullet personnalisé ────────────────────────────
static void spawnBossBullet(World& w, float ox, float oy,
                             float dx, float dy,
                             float spd, float dmg,
                             BossType btype,
                             BulletEffect fx=BulletEffect::NONE,
                             float aoeR=0, float aoeDmg=0)
{
    Entity e = w.createEntity();
    PositionComponent p; p.x=ox; p.y=oy; w.addComponent(e,p);
    BulletComponent b;
    b.vx=dx*spd; b.vy=dy*spd; b.speed=spd; b.damage=dmg;
    b.isEnemy=true; b.effect=fx; b.range=1400.f;
    b.aoeRadius=aoeR; b.aoeDamage=aoeDmg;
    w.addComponent(e,b);
    float angle=std::atan2(dy,dx)*180.f/3.14159f;
    BulletStyle st=bossStyle(btype);
    SpriteComponent s;
    s.texturePath=7; s.width=st.width; s.height=st.height;
    s.tag=EntityTag::BULLET_EN; s.layer=3; s.rotation=angle; s.tint=st.tint;
    w.addComponent(e,s);
    BoxColliderComponent c; c.isCircle=true; c.radius=st.radius;
    c.TagsCollided={(int)EntityTag::PLAYER};
    w.addComponent(e,c);
}

// ── Onde de choc (Boss 5) ─────────────────────────────────────
static void spawnShockwave(World& w, float ox, float oy) {
    // Anneau de balles dans toutes les directions
    for (int i=0; i<16; i++) {
        float ang = i*(6.2831f/16.f);
        spawnBossBullet(w,ox,oy,std::cos(ang),std::sin(ang),
                        180.f,15.f,BossType::SHOCKWAVE);
    }
    // Effet visuel
    Entity fx=w.createEntity();
    PositionComponent fp; fp.x=ox; fp.y=oy; w.addComponent(fx,fp);
    CircleComponent cc; cc.radius=120.f; cc.inlineColor={255,230,0,60};
    cc.outlineColor={255,230,0,200}; cc.tickness=4.f; cc.layer=5;
    cc.tag=EntityTag::EXPLOSION; w.addComponent(fx,cc);
    TimeComponent t; t.duration=0.4f; w.addComponent(fx,t);
}

// ── Heal sbires (Boss 5) ──────────────────────────────────────
static void healMinions(World& w, float cx, float cy, float amount) {
    for (Entity e : w.getContainer<EnemyAIComponent>().getEntities()) {
        auto* ai=w.getComponent<EnemyAIComponent>(e);
        auto* hp=w.getComponent<HealthComponent>(e);
        auto* pos=w.getComponent<PositionComponent>(e);
        if (!ai||!hp||!pos||ai->isBoss) continue;
        float dx=cx-pos->x,dy=cy-pos->y;
        if (std::sqrt(dx*dx+dy*dy)<700.f) {
            hp->hp=std::min(hp->maxHp,hp->hp+amount);
            // Flash vert heal
            if (auto* spr=w.getComponent<SpriteComponent>(e))
                spr->tint={100,255,100,255};
        }
    }
}

// ── Upgrade sbires (Boss 6 Final) ─────────────────────────────
static void upgradeMinions(World& w) {
    for (Entity e : w.getContainer<EnemyAIComponent>().getEntities()) {
        auto* ai=w.getComponent<EnemyAIComponent>(e);
        auto* hp=w.getComponent<HealthComponent>(e);
        auto* spr=w.getComponent<SpriteComponent>(e);
        if (!ai||!hp||!spr||ai->isBoss) continue;
        // Sprite plus effrayant (boss_tex_base pour les sbires)
        spr->texturePath=BOSS_TEX_BASE+3;  // boss_03 = sprite affreux
        spr->scale=std::min(spr->scale*1.4f,2.5f);
        spr->tint={200,50,255,255};
        // Stats améliorées
        ai->speed      *= 1.5f;
        ai->bulletSpeed*= 1.4f;
        ai->damage     *= 1.3f;
        ai->fireRate   *= 0.7f;
        hp->maxHp      *= 1.6f;
        hp->hp         *= 1.6f;
    }
}

// ── Spawn ennemi normal ────────────────────────────────────────
static void spawnNormal(World& w, int wave) {
    Entity player=findPlayer(w);
    float cx=SPAWN_CENTER_X, cy=SPAWN_CENTER_Y;
    if (auto* pp=w.getComponent<PositionComponent>(player)){ cx=pp->x; cy=pp->y; }
    std::uniform_real_distribution<float> ang(0.f,6.2831f);
    std::uniform_real_distribution<float> rad(600.f,950.f);
    float a=ang(rngW),r=rad(rngW);
    float px=std::max(80.f,std::min(cx+std::cos(a)*r,(float)(MAP_WORLD_W-80)));
    float py=std::max(80.f,std::min(cy+std::sin(a)*r,(float)(MAP_WORLD_H-80)));

    Entity e=w.createEntity();
    PositionComponent pos; pos.x=px; pos.y=py; w.addComponent(e,pos);

    float wf=(float)wave;
    float hp=50.f+wf*15.f;
    HealthComponent hpc; hpc.hp=hpc.maxHp=hp; w.addComponent(e,hpc);

    int tex=ENEMY_TEX_BASE+((wave-1)%ENEMY_TEX_COUNT);
    SpriteComponent spr;
    spr.texturePath=tex; spr.width=64; spr.height=64;
    spr.scale=1.2f; spr.offsetX=-38; spr.offsetY=-38;
    spr.tag=EntityTag::ENEMY; spr.layer=2; spr.tint=WHITE;
    w.addComponent(e,spr);

    BoxColliderComponent col; col.isCircle=true; col.radius=20.f;
    col.TagsCollided={(int)EntityTag::BULLET_PL,(int)EntityTag::PLAYER};
    w.addComponent(e,col);

    EnemyAIComponent ai;
    ai.speed       =35.f+wf*4.f;
    ai.fireRate    =std::max(1.2f,3.f-wf*0.1f);
    ai.damage      =0.08f;
    ai.bulletSpeed =65.f+wf*8.f;
    ai.burstCount  =1;
    ai.attackRange =450.f;
    ai.isBoss      =false;
    ai.bossType    =BossType::NONE;
    ai.bulletStyle ={{255,80,0,255},10,10,5};
    w.addComponent(e,ai);
}

// ── Spawn BOSS par vague ───────────────────────────────────────
static void spawnBoss(World& w, int wave) {
    Entity player=findPlayer(w);
    float cx=SPAWN_CENTER_X, cy=SPAWN_CENTER_Y;
    if (auto* pp=w.getComponent<PositionComponent>(player)){ cx=pp->x; cy=pp->y; }
    // Boss spawn à 700-800px du joueur
    std::uniform_real_distribution<float> ang(0.f,6.2831f);
    float a=ang(rngW),r=750.f;
    float px=std::max(100.f,std::min(cx+std::cos(a)*r,(float)(MAP_WORLD_W-100)));
    float py=std::max(100.f,std::min(cy+std::sin(a)*r,(float)(MAP_WORLD_H-100)));

    Entity e=w.createEntity();
    PositionComponent pos; pos.x=px; pos.y=py; w.addComponent(e,pos);

    // HP boss : croissants + boss final très costaud
    float hpBase;
    float sz;
    float sc;
    if      (wave==6) { hpBase=4000.f; sz=120.f; sc=2.0f; }
    else if (wave==5) { hpBase=2000.f; sz=100.f; sc=1.7f; }
    else              { hpBase=500.f+wave*200.f; sz=80.f; sc=1.4f; }
    HealthComponent hpc; hpc.hp=hpc.maxHp=hpBase; w.addComponent(e,hpc);

    // Sprite boss (boss_00 à boss_05 selon vague)
    int tex=BOSS_TEX_BASE+std::min(wave-1,5);
    SpriteComponent spr;
    spr.texturePath=tex; spr.width=sz; spr.height=sz;
    spr.scale=sc; spr.offsetX=-sz*sc/2.f; spr.offsetY=-sz*sc/2.f;
    spr.tag=EntityTag::BOSS; spr.layer=4; spr.tint=WHITE;
    w.addComponent(e,spr);

    BoxColliderComponent col; col.isCircle=true;
    col.radius=wave==6?55.f:38.f;
    col.TagsCollided={(int)EntityTag::BULLET_PL,(int)EntityTag::PLAYER};
    w.addComponent(e,col);

    // Config EnemyAI selon type de boss
    EnemyAIComponent ai;
    ai.isBoss  =true;
    ai.bossType=(BossType)wave;
    ai.bossPhase=0;
    ai.attackRange=600.f;

    switch ((BossType)wave) {

    // ── Boss 1 : Shotgun violet ──────────────────────────────
    case BossType::SHOTGUN:
        ai.speed=45; ai.fireRate=1.2f; ai.damage=0.1f;
        ai.bulletSpeed=220; ai.burstCount=7; ai.spreadAngle=50;
        ai.bulletStyle={{160,0,220,255},10,10,5};
        break;

    // ── Boss 2 : Sniper blanc ────────────────────────────────
    case BossType::SNIPER:
        ai.speed=35; ai.fireRate=2.5f; ai.damage=0.18f;
        ai.bulletSpeed=950; ai.burstCount=1; ai.spreadAngle=0;
        ai.bulletStyle={{220,240,255,255},16,6,5};
        break;

    // ── Boss 3 : Bombes jaunes ───────────────────────────────
    case BossType::BOMB:
        ai.speed=40; ai.fireRate=2.0f; ai.damage=0.15f;
        ai.bulletSpeed=200; ai.burstCount=2; ai.spreadAngle=15;
        ai.bulletStyle={{255,230,0,255},16,16,8};
        break;

    // ── Boss 4 : Rockets rouge vif ───────────────────────────
    case BossType::ROCKET:
        ai.speed=55; ai.fireRate=0.8f; ai.damage=0.2f;
        ai.bulletSpeed=400; ai.burstCount=3; ai.spreadAngle=20;
        ai.bulletStyle={{255,20,20,255},14,8,6};
        break;

    // ── Boss 5 : Onde de choc + heal sbires ──────────────────
    case BossType::SHOCKWAVE:
        ai.speed=50; ai.fireRate=1.5f; ai.damage=0.15f;
        ai.bulletSpeed=200; ai.burstCount=2; ai.spreadAngle=15;
        ai.healInterval=8.f; ai.healTimer=0.f; ai.healCount=0;
        ai.bulletStyle={{255,230,0,255},18,18,9};
        break;

    // ── Boss 6 : FINAL ───────────────────────────────────────
    case BossType::FINAL:
        ai.speed=40; ai.fireRate=1.5f; ai.damage=0.15f;
        ai.bulletSpeed=300; ai.burstCount=4; ai.spreadAngle=25;
        ai.upgradeInterval=15.f; ai.upgradeTimer=0.f; ai.upgradeCount=0;
        ai.bulletStyle={{255,100,0,255},12,12,6};
        // Teinte rouge sang dès le départ
        spr.tint={220,30,30,255};
        w.addComponent(e,spr); // ré-applique tint
        break;

    default: break;
    }
    w.addComponent(e,ai);
}

// ════════════════════════════════════════════════════════════════
// WAVE SYSTEM UPDATE
// ════════════════════════════════════════════════════════════════
void WaveSystem::update(float dt) {
    if (!world||!_sm) return;
    auto& waves=world->getContainer<WaveComponent>();
    for (Entity e : waves.getEntities()) {
        auto* wc=waves.get(e); if(!wc) continue;

        if (wc->state==WaveState::VICTORY) {
            _sm->changeScene(SceneType::GAMEOVER); return;
        }

        // ── Entre vagues ────────────────────────────────────
        if (wc->state==WaveState::BETWEEN_WAVES) {
            wc->betweenTimer-=dt;
            if (wc->betweenTimer<=0.f) {
                wc->currentWave++;
                if (wc->currentWave>wc->maxWaves) { wc->state=WaveState::VICTORY; return; }
                wc->enemiesLeft=8*wc->currentWave;
                wc->enemiesAlive=0;
                wc->bossSpawned=false;
                wc->betweenTimer=wc->betweenMax;
                wc->spawnTimer=0.f;
                wc->state=WaveState::FIGHTING;
            }
            return;
        }

        // ── Spawn ennemis normaux ────────────────────────────
        wc->spawnTimer+=dt;
        if (wc->spawnTimer>=wc->spawnRate && wc->enemiesLeft>0) {
            wc->spawnTimer=0.f;
            bool isLast=(wc->enemiesLeft==1);
            bool doBoss=(isLast&&!wc->bossSpawned);
            if (doBoss) { wc->bossSpawned=true; spawnBoss(*world,wc->currentWave); }
            else spawnNormal(*world,wc->currentWave);
            wc->enemiesLeft--;
        }

        // Compter vivants
        wc->enemiesAlive=(int)world->getContainer<EnemyAIComponent>().getEntities().size();
        if (wc->enemiesLeft==0&&wc->enemiesAlive==0) {
            wc->betweenTimer=wc->betweenMax;
            wc->state=WaveState::BETWEEN_WAVES;
        }
    }
}
