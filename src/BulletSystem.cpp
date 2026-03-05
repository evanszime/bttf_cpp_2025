#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rngB(99);

// Effets visuels à la mort d'un ennemi selon arme
static void spawnDeathEffect(World& w, float x, float y, BulletEffect fx) {
    Entity e=w.createEntity();
    PositionComponent p; p.x=x; p.y=y; w.addComponent(e,p);
    TimeComponent t;
    CircleComponent c; c.layer=6; c.tag=EntityTag::EXPLOSION;

    switch(fx) {
    case BulletEffect::EXPLOSION:
        // Grande explosion orange + onde de choc
        c.radius=90.f; c.inlineColor={255,100,0,200};
        c.outlineColor={255,220,0,255}; c.tickness=5.f; t.duration=0.5f;
        break;
    case BulletEffect::PIERCE:
        // Éclat bleu électrique
        c.radius=35.f; c.inlineColor={80,160,255,220};
        c.outlineColor={200,230,255,255}; c.tickness=3.f; t.duration=0.3f;
        break;
    case BulletEffect::BURST:
        // Douilles en gerbe
        c.radius=22.f; c.inlineColor={255,200,50,200};
        c.outlineColor={255,255,150,255}; c.tickness=2.f; t.duration=0.2f;
        break;
    default:
        // Pistol : petit flash blanc
        c.radius=18.f; c.inlineColor={255,255,200,180};
        c.outlineColor={255,255,255,200}; c.tickness=2.f; t.duration=0.18f;
        break;
    }
    w.addComponent(e,c); w.addComponent(e,t);

    // Ajouter quelques particules secondaires
    std::uniform_real_distribution<float> rd(-40.f,40.f);
    for(int i=0;i<4;i++){
        Entity pe=w.createEntity();
        PositionComponent pp; pp.x=x+rd(rngB); pp.y=y+rd(rngB); w.addComponent(pe,pp);
        CircleComponent pc; pc.radius=8.f; pc.layer=6; pc.tag=EntityTag::EXPLOSION;
        pc.inlineColor=c.inlineColor; pc.outlineColor=c.outlineColor; pc.tickness=1.f;
        w.addComponent(pe,pc);
        TimeComponent pt; pt.duration=t.duration*0.6f; w.addComponent(pe,pt);
    }
}

// Spawn un drop au sol (ammo/hp/bouclier) avec proba
static void spawnDrop(World& w, float x, float y) {
    std::uniform_int_distribution<int> d(0,99);
    int r=d(rngB);
    // 40% ammo, 25% hp, 15% bouclier, 20% rien
    EntityTag tag; int tex;
    if(r<40)       { tag=EntityTag::PICKUP_AMMO;   tex=10; }
    else if(r<65)  { tag=EntityTag::PICKUP_HP;     tex=14; }  // xp_orb vert = soin
    else if(r<80)  { tag=EntityTag::PICKUP_SHIELD; tex=6; }  // bullet_player coloré en bleu
    else return;   // pas de drop

    Entity e=w.createEntity();
    PositionComponent p; p.x=x; p.y=y; w.addComponent(e,p);
    SpriteComponent s; s.texturePath=tex; s.width=20; s.height=20;
    s.scale=1.f; s.tag=tag; s.layer=1;
    s.offsetX=-10; s.offsetY=-10;
    // Couleur distincte
    if(tag==EntityTag::PICKUP_AMMO)   s.tint={255,220,50,255};
    if(tag==EntityTag::PICKUP_HP)     s.tint={50,255,100,255};
    if(tag==EntityTag::PICKUP_SHIELD) s.tint={80,150,255,255};
    w.addComponent(e,s);
    BoxColliderComponent c2; c2.isCircle=true; c2.radius=14.f;
    c2.TagsCollided={(int)EntityTag::PLAYER}; w.addComponent(e,c2);
}

void BulletSystem::update(float dt) {
    if(!world) return;
    auto& bullets   = world->getContainer<BulletComponent>();
    auto& positions = world->getContainer<PositionComponent>();
    auto& colliders = world->getContainer<BoxColliderComponent>();
    std::vector<Entity> toRemove;

    // ── Nettoyage effets temporaires ──────────────────────────
    auto& times=world->getContainer<TimeComponent>();
    std::vector<Entity> expiredFx;
    for(Entity e:times.getEntities()){
        auto*t=times.get(e); if(!t) continue;
        t->elapsed+=dt;
        if(t->elapsed>=t->duration) expiredFx.push_back(e);
    }
    for(Entity e:expiredFx) world->removeEntity(e);

    // ── Déplacement et collisions balles ─────────────────────
    std::vector<Entity> bulletList=bullets.getEntities();
    for(Entity e:bulletList){
        auto*b=bullets.get(e);
        auto*pos=positions.get(e);
        if(!b||!pos) continue;

        pos->x+=b->vx*dt;
        pos->y+=b->vy*dt;
        float speed=std::sqrt(b->vx*b->vx+b->vy*b->vy);
        b->traveled+=speed*dt;

        // Hors portée ou écran
        if(b->traveled>=b->range||
           pos->x<-300||pos->x>2220||pos->y<-300||pos->y>1380){
            toRemove.push_back(e); continue;
        }

        auto*col=colliders.get(e);
        if(!col||col->EntityCollided.empty()) continue;

        bool destroyed=false;
        for(int hitId:col->EntityCollided){
            Entity hit=static_cast<Entity>(hitId);
            auto*hitHp=world->getComponent<HealthComponent>(hit);
            if(!hitHp||hitHp->isDead) continue;

            // ── AoE Bomb ────────────────────────────────────
            if(b->effect==BulletEffect::EXPLOSION && b->aoeRadius>0){
                // Dégâts directs sur la cible
                hitHp->hp-=b->damage;
                // Spawn effet explosion
                spawnDeathEffect(*world,pos->x,pos->y,BulletEffect::EXPLOSION);
                // Splash sur tous les ennemis dans le rayon
                float R=b->aoeRadius;
                for(Entity en:world->getContainer<EnemyAIComponent>().getEntities()){
                    auto*ep=world->getComponent<PositionComponent>(en);
                    if(!ep) continue;
                    float dx=ep->x-pos->x,dy=ep->y-pos->y;
                    if(std::sqrt(dx*dx+dy*dy)<=R){
                        auto*eh=world->getComponent<HealthComponent>(en);
                        if(eh&&!eh->isDead) eh->hp-=b->aoeDamage;
                    }
                }
                // Knockback autour de l'explosion
                for(Entity en:world->getContainer<RigidbodyComponent>().getEntities()){
                    auto*ep=world->getComponent<PositionComponent>(en);
                    if(!ep) continue;
                    float dx=ep->x-pos->x,dy=ep->y-pos->y;
                    float d=std::sqrt(dx*dx+dy*dy);
                    if(d<R&&d>0){
                        auto*rb=world->getComponent<RigidbodyComponent>(en);
                        if(rb){rb->vx+=(dx/d)*300.f;rb->vy+=(dy/d)*300.f;}
                    }
                }
                toRemove.push_back(e); destroyed=true; break;
            }

            // ── Pierce Sniper ────────────────────────────────
            if(b->effect==BulletEffect::PIERCE){
                hitHp->hp-=b->damage;
                spawnDeathEffect(*world,pos->x,pos->y,BulletEffect::PIERCE);
                if(b->pierceLeft>0){ b->pierceLeft--; continue; }
                toRemove.push_back(e); destroyed=true; break;
            }

            // ── Standard (Pistol, Shotgun, AK47) ────────────
            hitHp->hp-=b->damage;
            spawnDeathEffect(*world,pos->x,pos->y,b->effect);
            toRemove.push_back(e); destroyed=true; break;
        }
        (void)destroyed;
    }

    // ── Marquer ennemis morts + drops ────────────────────────
    std::vector<Entity> deadEnemies;
    for(Entity e:world->getContainer<EnemyAIComponent>().getEntities()){
        auto*hp=world->getComponent<HealthComponent>(e);
        if(hp&&hp->hp<=0.f&&!hp->isDead){
            hp->isDead=true;
            deadEnemies.push_back(e);
        }
    }
    for(Entity e:deadEnemies){
        auto*pos=positions.get(e);
        if(pos){
            // XP orb toujours
            Entity orb=world->createEntity();
            PositionComponent op; op.x=pos->x; op.y=pos->y; world->addComponent(orb,op);
            SpriteComponent os; os.texturePath=14; os.width=12; os.height=12;
            os.tag=EntityTag::XP_ORB; os.layer=1; os.tint=GREEN;
            os.offsetX=-6; os.offsetY=-6; world->addComponent(orb,os);
            BoxColliderComponent oc; oc.isCircle=true; oc.radius=10.f;
            oc.TagsCollided={(int)EntityTag::PLAYER}; world->addComponent(orb,oc);
            // Drop aléatoire (ammo/hp/bouclier)
            spawnDrop(*world,pos->x+15,pos->y);
        }
        // Compter kill
        for(Entity p:world->getContainer<PlayerComponent>().getEntities())
            if(auto*pl=world->getComponent<PlayerComponent>(p)) pl->kills++;
        world->removeEntity(e);
    }

    for(Entity e:toRemove) world->removeEntity(e);
}
