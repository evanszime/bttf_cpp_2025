#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rngE(55);

static Entity findPlayer(World& w){
    for(Entity e:w.getContainer<PlayerComponent>().getEntities()) return e;
    return Entity(0);
}

static void fireBullet(World& w,float ox,float oy,float dx,float dy,
                        float spd,float dmg,float range=800.f)
{
    Entity e=w.createEntity();
    PositionComponent p; p.x=ox; p.y=oy; w.addComponent(e,p);
    BulletComponent b; b.vx=dx*spd; b.vy=dy*spd; b.speed=spd;
    b.damage=dmg; b.isEnemy=true; b.range=range; w.addComponent(e,b);
    float angle=std::atan2(dy,dx)*180.f/3.14159f;
    SpriteComponent s; s.texturePath=4; s.width=10; s.height=10;
    s.tag=EntityTag::BULLET_EN; s.layer=3; s.rotation=angle;
    s.tint={255,80,0,255}; w.addComponent(e,s);
    BoxColliderComponent c; c.isCircle=true; c.radius=5.f;
    c.TagsCollided={(int)EntityTag::PLAYER}; w.addComponent(e,c);
}

void EnemySystem::update(float dt){
    if(!world) return;
    auto& enemies  =world->getContainer<EnemyAIComponent>();
    auto& positions=world->getContainer<PositionComponent>();

    Entity player=findPlayer(*world);
    auto*playerPos=positions.get(player);

    for(Entity e:enemies.getEntities()){
        auto*ai=enemies.get(e);
        auto*pos=positions.get(e);
        auto*hp=world->getComponent<HealthComponent>(e);
        if(!ai||!pos||!hp||hp->isDead) continue;

        if(!playerPos) continue;
        float dx=playerPos->x-pos->x;
        float dy=playerPos->y-pos->y;
        float dist=std::sqrt(dx*dx+dy*dy);

        // ── Mouvement ──────────────────────────────────────
        if(dist>35.f){
            float spd=ai->speed;
            // Boss phase 2 : +30% vitesse
            if(ai->isBoss&&ai->bossPhase==1) spd*=1.3f;
            pos->x+=(dx/dist)*spd*dt;
            pos->y+=(dy/dist)*spd*dt;
        }

        // ── Phase boss (HP<50%) ───────────────────────────
        if(ai->isBoss&&hp->hp<hp->maxHp*0.5f&&ai->bossPhase==0){
            ai->bossPhase=1;
            ai->bulletSpeed*=1.4f;
            ai->spreadAngle=30.f;
            ai->burstCount=5;
            ai->fireRate=0.9f;
            // Flash rouge
            if(auto*spr=world->getComponent<SpriteComponent>(e))
                spr->tint={255,50,50,255};
        }

        // ── Rotation ──────────────────────────────────────
        if(auto*spr=world->getComponent<SpriteComponent>(e))
            spr->rotation=std::atan2(dy,dx)*180.f/3.14159f;

        // ── Tir ───────────────────────────────────────────
        ai->lastFireTime+=dt;
        if(dist<ai->attackRange&&ai->lastFireTime>=ai->fireRate){
            ai->lastFireTime=0.f;
            float bx=dx/dist, by=dy/dist;
            float sp=ai->spreadAngle*(3.14159f/180.f);
            for(int i=0;i<ai->burstCount;i++){
                float angle=std::atan2(by,bx);
                if(ai->burstCount>1)
                    angle+=-sp/2.f+(sp/(ai->burstCount-1))*i;
                fireBullet(*world,pos->x,pos->y,
                           std::cos(angle),std::sin(angle),
                           ai->bulletSpeed,ai->damage);
            }
        }
    }
}
