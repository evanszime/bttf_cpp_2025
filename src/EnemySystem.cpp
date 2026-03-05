#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rngE(55);

static Entity findPlayer(World& w) {
    for (Entity e : w.getContainer<PlayerComponent>().getEntities()) return e;
    return Entity(0);
}

static void normalize(float& x, float& y) {
    float l=std::sqrt(x*x+y*y); if(l>0){x/=l;y/=l;}
}

// ── Spawn balle ennemie avec style ────────────────────────────
static void spawnEnemyBullet(World& w, float ox, float oy,
                              float dx, float dy,
                              float spd, float dmg,
                              BulletStyle style,
                              BulletEffect fx=BulletEffect::NONE,
                              float aoeR=0, float aoeDmg=0,
                              float range=900.f)
{
    Entity e=w.createEntity();
    PositionComponent p; p.x=ox; p.y=oy; w.addComponent(e,p);
    BulletComponent b;
    b.vx=dx*spd; b.vy=dy*spd; b.speed=spd; b.damage=dmg;
    b.isEnemy=true; b.effect=fx; b.range=range;
    b.aoeRadius=aoeR; b.aoeDamage=aoeDmg;
    w.addComponent(e,b);
    float angle=std::atan2(dy,dx)*180.f/3.14159f;
    SpriteComponent s;
    s.texturePath=7; s.width=style.width*1.5f; s.height=style.height*1.5f;
    s.tag=EntityTag::BULLET_EN; s.layer=5; s.rotation=angle; s.tint=style.tint;
    w.addComponent(e,s);
    BoxColliderComponent c; c.isCircle=true; c.radius=style.radius;
    c.TagsCollided={(int)EntityTag::PLAYER};
    w.addComponent(e,c);
}

// ── Onde de choc boss 5 ────────────────────────────────────────
static void doShockwave(World& w, float ox, float oy, BulletStyle st) {
    for (int i=0; i<16; i++) {
        float a=i*(6.2831f/16.f);
        spawnEnemyBullet(w,ox,oy,std::cos(a),std::sin(a),
                         180.f,12.f,st);
    }
    // Effet cercle jaune
    Entity fx=w.createEntity();
    PositionComponent fp; fp.x=ox; fp.y=oy; w.addComponent(fx,fp);
    CircleComponent cc; cc.radius=150.f; cc.inlineColor={255,230,0,40};
    cc.outlineColor={255,230,0,210}; cc.tickness=5.f; cc.layer=6;
    cc.tag=EntityTag::EXPLOSION; w.addComponent(fx,cc);
    TimeComponent t; t.duration=0.45f; w.addComponent(fx,t);
}

// ── Heal sbires (boss 5) ───────────────────────────────────────
static void healMinions(World& w, float cx, float cy, float amount) {
    for (Entity e : w.getContainer<EnemyAIComponent>().getEntities()) {
        auto* ai=w.getComponent<EnemyAIComponent>(e);
        auto* hp=w.getComponent<HealthComponent>(e);
        auto* pos=w.getComponent<PositionComponent>(e);
        if (!ai||!hp||!pos||ai->isBoss||hp->isDead) continue;
        float dx=cx-pos->x, dy=cy-pos->y;
        if (std::sqrt(dx*dx+dy*dy)<800.f) {
            hp->hp=std::min(hp->maxHp,hp->hp+amount);
            if (auto* spr=w.getComponent<SpriteComponent>(e))
                spr->tint={80,255,120,255}; // flash vert heal
        }
    }
}

// ── Upgrade sbires (boss 6) ────────────────────────────────────
static void upgradeMinions(World& w) {
    for (Entity e : w.getContainer<EnemyAIComponent>().getEntities()) {
        auto* ai=w.getComponent<EnemyAIComponent>(e);
        auto* hp=w.getComponent<HealthComponent>(e);
        auto* spr=w.getComponent<SpriteComponent>(e);
        if (!ai||!hp||!spr||ai->isBoss||hp->isDead) continue;
        // Sprite plus affreux
        static const int UPGRADED_TEX=28; // boss_03 (costaud)
        spr->texturePath=UPGRADED_TEX;
        spr->scale=std::min(spr->scale*1.45f,2.4f);
        spr->tint={180,30,255,255};  // violet sang
        // Stats
        ai->speed      =std::min(ai->speed*1.5f,220.f);
        ai->bulletSpeed*=1.4f;
        ai->damage     *=1.4f;
        ai->fireRate   *=0.65f;
        if (hp->hp>0) { hp->maxHp*=1.5f; hp->hp=std::min(hp->hp*1.5f,hp->maxHp); }
    }
}

// ── Phase boss final (toutes les 20% de vie) ──────────────────
static int getFinalBossPhase(float hpRatio) {
    if (hpRatio>0.80f) return 0;
    if (hpRatio>0.60f) return 1;
    if (hpRatio>0.40f) return 2;
    if (hpRatio>0.20f) return 3;
    return 4;
}

// ── Config phase boss final ────────────────────────────────────
struct FinalPhaseConfig {
    float speed, fireRate, bulletSpeed, spreadAngle;
    int   burstCount;
    Color tint;
    const char* bulletType; // "all" = tire tout type de balle
};

static const FinalPhaseConfig FINAL_PHASES[5] = {
    // Phase 0 (100-80%)
    {40,1.5f,300,25,4, {220,30,30,255},   "all"},
    // Phase 1 (80-60%)  : +vitesse, +balles
    {60,1.0f,380,30,6, {255,40,40,255},   "all"},
    // Phase 2 (60-40%)
    {80,0.7f,460,35,8, {255,80,0,255},    "all"},
    // Phase 3 (40-20%)
    {110,0.5f,540,40,10,{255,120,0,255},  "all"},
    // Phase 4 (20-0%)  : RAGE TOTALE
    {140,0.3f,650,50,14,{255,200,0,255},  "all"},
};

// Types de balles pour le boss final (cycle)
static const int N_BOSS_TYPES=5;
static BulletStyle finalBulletStyles[N_BOSS_TYPES] = {
    {{160,0,220,255},10,10,5},   // shotgun violet
    {{220,240,255,255},16,6,5},  // sniper blanc
    {{255,230,0,255},16,16,8},   // bomb jaune
    {{255,20,20,255},14,8,6},    // rocket rouge
    {{255,100,200,255},12,12,6}, // rose choquant
};

// ════════════════════════════════════════════════════════════════
void EnemySystem::update(float dt) {
    if (!world) return;
    auto& enemies   = world->getContainer<EnemyAIComponent>();
    auto& positions = world->getContainer<PositionComponent>();

    Entity player=findPlayer(*world);
    auto* playerPos=positions.get(player);

    for (Entity e : enemies.getEntities()) {
        auto* ai =enemies.get(e);
        auto* pos=positions.get(e);
        auto* hp =world->getComponent<HealthComponent>(e);
        auto* spr=world->getComponent<SpriteComponent>(e);
        if (!ai||!pos||!hp||hp->isDead) continue;
        if (!playerPos) continue;

        float dx=playerPos->x-pos->x;
        float dy=playerPos->y-pos->y;
        float dist=std::sqrt(dx*dx+dy*dy);

        // ── Mouvement ──────────────────────────────────────
        if (dist>40.f) {
            pos->x+=(dx/dist)*ai->speed*dt;
            pos->y+=(dy/dist)*ai->speed*dt;
        }

        // ── Rotation vers le joueur ────────────────────────
        if (spr) spr->rotation=std::atan2(dy,dx)*180.f/3.14159f;

        // ══════════════════════════════════════════════════
        // LOGIQUE BOSS SPÉCIALE
        // ══════════════════════════════════════════════════
        if (ai->isBoss) {
            float hpRatio=hp->maxHp>0?hp->hp/hp->maxHp:0;

            // ── Boss 6 FINAL : phases de rage ───────────────
            if (ai->bossType==BossType::FINAL) {
                int phase=getFinalBossPhase(hpRatio);
                if (phase!=ai->bossPhase) {
                    ai->bossPhase=phase;
                    const FinalPhaseConfig& cfg=FINAL_PHASES[phase];
                    ai->speed       =cfg.speed;
                    ai->fireRate    =cfg.fireRate;
                    ai->bulletSpeed =cfg.bulletSpeed;
                    ai->spreadAngle =cfg.spreadAngle;
                    ai->burstCount  =cfg.burstCount;
                    if (spr) spr->tint=cfg.tint;
                    // Flash rouge rage
                    Entity fx=world->createEntity();
                    PositionComponent fp; fp.x=pos->x; fp.y=pos->y; world->addComponent(fx,fp);
                    CircleComponent cc; cc.radius=200.f; cc.inlineColor={255,0,0,40};
                    cc.outlineColor={255,50,0,200}; cc.tickness=6.f; cc.layer=6;
                    cc.tag=EntityTag::EXPLOSION; world->addComponent(fx,cc);
                    TimeComponent t; t.duration=0.6f; world->addComponent(fx,t);
                }
                // Upgrade sbires quand possible (2 fois max)
                ai->upgradeTimer+=dt;
                if (ai->upgradeTimer>=ai->upgradeInterval && ai->upgradeCount<2) {
                    ai->upgradeTimer=0.f; ai->upgradeCount++;
                    upgradeMinions(*world);
                }
            }

            // ── Boss 1 basique (phase 2 à 50% HP) ───────────
            if (ai->bossType==BossType::SHOTGUN && ai->bossPhase==0 && hpRatio<0.5f) {
                ai->bossPhase=1;
                ai->burstCount=11; ai->spreadAngle=65; ai->fireRate=0.9f;
                ai->bulletSpeed*=1.2f;
                if (spr) spr->tint={200,0,255,255};
            }
            // ── Boss 2 sniper (phase 2 : rafale triple) ───────
            if (ai->bossType==BossType::SNIPER && ai->bossPhase==0 && hpRatio<0.5f) {
                ai->bossPhase=1;
                ai->burstCount=3; ai->spreadAngle=8; ai->fireRate=1.8f;
                if (spr) spr->tint={180,220,255,255};
            }
            // ── Boss 3 bomb (phase 2 : triple bombes) ─────────
            if (ai->bossType==BossType::BOMB && ai->bossPhase==0 && hpRatio<0.5f) {
                ai->bossPhase=1;
                ai->burstCount=4; ai->spreadAngle=30; ai->fireRate=1.5f;
                if (spr) spr->tint={255,255,50,255};
            }
            // ── Boss 4 rocket (phase 2 : rocket spam) ─────────
            if (ai->bossType==BossType::ROCKET && ai->bossPhase==0 && hpRatio<0.5f) {
                ai->bossPhase=1;
                ai->burstCount=5; ai->spreadAngle=30; ai->fireRate=0.5f;
                if (spr) spr->tint={255,0,0,255};
            }
            // ── Boss 5 shockwave : heal + onde ────────────────
            if (ai->bossType==BossType::SHOCKWAVE) {
                ai->healTimer+=dt;
                if (ai->healTimer>=ai->healInterval && ai->healCount<2) {
                    ai->healTimer=0.f; ai->healCount++;
                    healMinions(*world,pos->x,pos->y,20.f);
                    // Onde de choc en bonus
                    doShockwave(*world,pos->x,pos->y,ai->bulletStyle);
                }
                if (ai->bossPhase==0 && hpRatio<0.5f) {
                    ai->bossPhase=1;
                    ai->fireRate=1.0f; ai->burstCount=4; ai->spreadAngle=25;
                    ai->healInterval=5.f;
                }
            }
        }

        // ══════════════════════════════════════════════════
        // TIR
        // ══════════════════════════════════════════════════
        ai->lastFireTime+=dt;
        if (dist<ai->attackRange && ai->lastFireTime>=ai->fireRate) {
            ai->lastFireTime=0.f;
            float bx=dx/dist, by=dy/dist;
            float sp=ai->spreadAngle*(3.14159f/180.f);

            // ── Boss 6 FINAL : tir cyclique multi-type ───────
            if (ai->isBoss && ai->bossType==BossType::FINAL) {
                // Cycle parmi les styles selon la phase
                int styleIdx=ai->bossPhase%N_BOSS_TYPES;
                BulletStyle st=finalBulletStyles[styleIdx];
                // Effet selon style
                BulletEffect fx=BulletEffect::NONE;
                float aoeR=0,aoeDmg=0;
                if (styleIdx==2||styleIdx==1){ fx=BulletEffect::EXPLOSION; aoeR=100; aoeDmg=30; }
                if (styleIdx==1){ fx=BulletEffect::PIERCE; aoeR=0; aoeDmg=0; }
                for (int i=0;i<ai->burstCount;i++) {
                    float angle=std::atan2(by,bx);
                    if (ai->burstCount>1) angle+=-sp/2.f+(sp/(ai->burstCount-1))*i;
                    spawnEnemyBullet(*world,pos->x,pos->y,
                                     std::cos(angle),std::sin(angle),
                                     ai->bulletSpeed,ai->damage,st,fx,aoeR,aoeDmg);
                }
                // En plus : cercle de balles en phase rage
                if (ai->bossPhase>=3) {
                    for (int i=0;i<8;i++) {
                        float a=i*(6.2831f/8.f);
                        BulletStyle st2=finalBulletStyles[(styleIdx+2)%N_BOSS_TYPES];
                        spawnEnemyBullet(*world,pos->x,pos->y,
                                         std::cos(a),std::sin(a),
                                         ai->bulletSpeed*0.7f,ai->damage*0.8f,st2);
                    }
                }
            }

            // ── Boss 3 & 5 : BOMBE avec AoE ─────────────────
            else if (ai->isBoss &&
                     (ai->bossType==BossType::BOMB||ai->bossType==BossType::SHOCKWAVE)) {
                for (int i=0;i<ai->burstCount;i++) {
                    float angle=std::atan2(by,bx);
                    if (ai->burstCount>1) angle+=-sp/2.f+(sp/(ai->burstCount-1))*i;
                    spawnEnemyBullet(*world,pos->x,pos->y,
                                     std::cos(angle),std::sin(angle),
                                     ai->bulletSpeed,ai->damage,
                                     ai->bulletStyle,BulletEffect::EXPLOSION,
                                     90.f,20.f);
                }
                // Boss 5 : onde de choc périodique via stateTimer
                if (ai->bossType==BossType::SHOCKWAVE) {
                    ai->stateTimer+=dt;
                    if (ai->stateTimer>=6.f) {
                        ai->stateTimer=0.f;
                        doShockwave(*world,pos->x,pos->y,ai->bulletStyle);
                    }
                }
            }

            // ── Boss 2 SNIPER : balle pierçante ──────────────
            else if (ai->isBoss && ai->bossType==BossType::SNIPER) {
                for (int i=0;i<ai->burstCount;i++) {
                    float angle=std::atan2(by,bx);
                    if (ai->burstCount>1) angle+=-sp/2.f+(sp/(ai->burstCount-1))*i;
                    spawnEnemyBullet(*world,pos->x,pos->y,
                                     std::cos(angle),std::sin(angle),
                                     ai->bulletSpeed,ai->damage,
                                     ai->bulletStyle,BulletEffect::PIERCE,0,0,2000.f);
                }
            }

            // ── Tous les autres (normal + boss 1 shotgun + boss 4 rocket) ──
            else {
                for (int i=0;i<ai->burstCount;i++) {
                    float angle=std::atan2(by,bx);
                    if (ai->burstCount>1) angle+=-sp/2.f+(sp/(ai->burstCount-1))*i;
                    BulletStyle st=ai->isBoss ? ai->bulletStyle : BulletStyle{{255,80,0,255},10,10,5};
                    spawnEnemyBullet(*world,pos->x,pos->y,
                                     std::cos(angle),std::sin(angle),
                                     ai->bulletSpeed,ai->damage,st);
                }
            }
        }
    }
}
