#include "../include/Engine.hpp"
#include <cmath>
#include <random>

// ================================================================
// HELPERS
// ================================================================
static std::mt19937 rng(42);

static void normalize(float& x, float& y){
    float l=std::sqrt(x*x+y*y); if(l>0){x/=l;y/=l;}
}

// Spawn une particule visuelle temporaire (cercle qui disparait)
static void spawnParticle(World& w, float x, float y,
                           Color col, float radius, float duration,
                           EntityTag tag=EntityTag::EXPLOSION)
{
    Entity e=w.createEntity();
    PositionComponent p; p.x=x; p.y=y; w.addComponent(e,p);
    CircleComponent c; c.radius=radius; c.inlineColor=col;
    c.outlineColor={255,255,200,200}; c.tickness=2.f; c.layer=5;
    c.tag=tag; w.addComponent(e,c);
    TimeComponent t; t.duration=duration; w.addComponent(e,t);
}

// Spawn un bullet avec tous ses paramètres
static Entity spawnBullet(World& w,
    float ox,float oy, float dx,float dy,
    float spd, float dmg, bool isEnemy,
    BulletEffect fx=BulletEffect::NONE,
    int pierce=0, float aoeR=0,float aoeDmg=0)
{
    Entity e=w.createEntity();
    PositionComponent pos; pos.x=ox; pos.y=oy; w.addComponent(e,pos);

    BulletComponent b;
    b.vx=dx*spd; b.vy=dy*spd; b.speed=spd; b.damage=dmg;
    b.isEnemy=isEnemy; b.effect=fx;
    b.pierceLeft=pierce; b.aoeRadius=aoeR; b.aoeDamage=aoeDmg;
    w.addComponent(e,b);

    float angle=std::atan2(dy,dx)*180.f/3.14159f;
    SpriteComponent spr;
    spr.texturePath=isEnemy?4:3;
    spr.width=10; spr.height=10;
    spr.tag=isEnemy?EntityTag::BULLET_EN:EntityTag::BULLET_PL;
    spr.layer=3; spr.rotation=angle;

    // Taille/couleur par effet
    if(fx==BulletEffect::EXPLOSION){ spr.width=18;spr.height=18;spr.tint=ORANGE; }
    else if(fx==BulletEffect::PIERCE){ spr.width=14;spr.height=6;spr.tint=SKYBLUE; }
    w.addComponent(e,spr);

    BoxColliderComponent col; col.isCircle=true;
    col.radius=(fx==BulletEffect::EXPLOSION)?8.f:5.f;
    col.TagsCollided=isEnemy
        ? std::vector<int>{(int)EntityTag::PLAYER}
        : std::vector<int>{(int)EntityTag::ENEMY,(int)EntityTag::BOSS};
    w.addComponent(e,col);
    return e;
}

// ================================================================
// 5 ARMES
// ================================================================

// 1) PISTOL - 20 dmg, 12 balles, semi-auto
class Pistol : public IWeapon {
    World* _w;
public:
    Pistol(World* w):_w(w){maxAmmo=50;currentAmmo=50;fireRate=2.0f;reloadTime=1.0f;}
    std::string getName() const override{return "Pistol";}
    std::string getDesc() const override{return "Semi-auto | 50 dmg | 12/12";}
    bool fire(float px,float py,float tx,float ty) override {
        if(!canFire()) return false;
        float dx=tx-px,dy=ty-py; normalize(dx,dy);
        spawnBullet(*_w,px,py,dx,dy,650.f,20.f,false);
        // Flash visuel
        spawnParticle(*_w,px,py,{255,255,100,200},8.f,0.06f);
        currentAmmo--; fireCooldown=fireRate; return true;
    }
};

// 2) SHOTGUN - 8dmg x7, 6 balles, cône 40°
class Shotgun : public IWeapon {
    World* _w;
public:
    Shotgun(World* w):_w(w){maxAmmo=6;currentAmmo=6;fireRate=0.85f;reloadTime=2.f;}
    std::string getName() const override{return "Shotgun";}
    std::string getDesc() const override{return "7 pellets | 8dmg each | 6/6";}
    bool fire(float px,float py,float tx,float ty) override {
        if(!canFire()) return false;
        float dx=tx-px,dy=ty-py; normalize(dx,dy);
        float base=std::atan2(dy,dx);
        float spread=40.f*(3.14159f/180.f);
        for(int i=0;i<7;i++){
            float a=base-spread/2.f+(spread/6.f)*i;
            std::uniform_real_distribution<float> jit(-0.05f,0.05f);
            spawnBullet(*_w,px,py,std::cos(a+jit(rng)),std::sin(a+jit(rng)),360.f,8.f,false);
        }
        // Blast visuel
        spawnParticle(*_w,px,py,{255,140,0,180},18.f,0.12f);
        currentAmmo--; fireCooldown=fireRate; return true;
    }
};

// 3) SNIPER - 60dmg, 5 balles, perforant x3
class Sniper : public IWeapon {
    World* _w;
public:
    Sniper(World* w):_w(w){maxAmmo=5;currentAmmo=5;fireRate=1.3f;reloadTime=2.5f;}
    std::string getName() const override{return "Sniper";}
    std::string getDesc() const override{return "Pierce x3 | 60dmg | 5/5";}
    bool fire(float px,float py,float tx,float ty) override {
        if(!canFire()) return false;
        float dx=tx-px,dy=ty-py; normalize(dx,dy);
        Entity e=spawnBullet(*_w,px,py,dx,dy,950.f,60.f,false,BulletEffect::PIERCE,3,0,0);
        auto*b=_w->getComponent<BulletComponent>(e); if(b) b->range=2000.f;
        // Traînée bleue
        spawnParticle(*_w,px,py,{100,180,255,220},12.f,0.08f);
        currentAmmo--; fireCooldown=fireRate; return true;
    }
};

// 4) AK47 - 12dmg, 30 balles, rafale auto
class AK47 : public IWeapon {
    World* _w;
public:
    AK47(World* w):_w(w){maxAmmo=30;currentAmmo=30;fireRate=0.1f;reloadTime=2.f;}
    std::string getName() const override{return "AK-47";}
    std::string getDesc() const override{return "Full-auto | 12dmg | 30/30";}
    bool fire(float px,float py,float tx,float ty) override {
        if(!canFire()) return false;
        float dx=tx-px,dy=ty-py; normalize(dx,dy);
        // Légère dispersion aléatoire
        std::uniform_real_distribution<float> jit(-0.06f,0.06f);
        float jx=jit(rng),jy=jit(rng);
        float nx=dx+jx,ny=dy+jy; normalize(nx,ny);
        spawnBullet(*_w,px,py,nx,ny,550.f,12.f,false,BulletEffect::BURST);
        // Douille visuelle (petit cercle doré rapide)
        spawnParticle(*_w,px+jit(rng)*10,py+jit(rng)*10,{255,200,50,200},5.f,0.08f);
        currentAmmo--; fireCooldown=fireRate; return true;
    }
};

// 5) BOMB LAUNCHER - 80dmg direct + 50dmg AoE 120px, 3 balles
class BombLauncher : public IWeapon {
    World* _w;
public:
    BombLauncher(World* w):_w(w){maxAmmo=3;currentAmmo=3;fireRate=1.8f;reloadTime=3.f;}
    std::string getName() const override{return "Bomb";}
    std::string getDesc() const override{return "AoE 120px | 80+50dmg | 3/3";}
    bool fire(float px,float py,float tx,float ty) override {
        if(!canFire()) return false;
        float dx=tx-px,dy=ty-py; normalize(dx,dy);
        Entity e=spawnBullet(*_w,px,py,dx,dy,200.f,80.f,false,
                              BulletEffect::EXPLOSION,0,120.f,50.f);
        auto*b=_w->getComponent<BulletComponent>(e); if(b) b->range=600.f;
        // Flash de lancement
        spawnParticle(*_w,px,py,{255,80,0,220},20.f,0.1f);
        currentAmmo--; fireCooldown=fireRate; return true;
    }
};

// ================================================================
// FACTORY
// ================================================================
std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world) {
    if(type=="shotgun")  return std::make_shared<Shotgun>(world);
    if(type=="sniper")   return std::make_shared<Sniper>(world);
    if(type=="ak47")     return std::make_shared<AK47>(world);
    if(type=="bomb")     return std::make_shared<BombLauncher>(world);
    return std::make_shared<Pistol>(world);  // défaut
}

// ================================================================
// SYSTEM UPDATE
// ================================================================
void WeaponSystem::update(float dt) {
    if(!world) return;
    auto& weapons=world->getContainer<WeaponComponent>();
    for(Entity e:weapons.getEntities()){
        auto*wc=weapons.get(e); if(!wc) continue;
        // Mettre à jour les 2 slots
        for(auto& slot:wc->slots)
            if(slot.weapon) slot.weapon->update(dt);
    }
}
