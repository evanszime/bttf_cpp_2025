#pragma once
#include "Engine.hpp"
#include "IGame.hpp"
#include <memory>
#include <sstream>
#include <cmath>

// Forward
std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

// ================================================================
// MENU SCENE
// ================================================================
class MenuScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _anim=0;
public:
    MenuScene(SceneManager& sm, GlobalComponent& g):Scene("Menu"),_sm(&sm),_g(&g){}
    void onEnter() override {_anim=0;}
    void onExit()  override {}

    void update(float dt) override {
        _anim+=dt;
        if(!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
        Vector2 m=GetMousePosition();
        float nx=m.x/GetScreenWidth(), ny=m.y/GetScreenHeight();
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.39f&&ny<=0.46f) _sm->changeScene(SceneType::GAME);
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.49f&&ny<=0.56f) _sm->changeScene(SceneType::SETTINGS);
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.59f&&ny<=0.66f) CloseWindow();
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();

        // Fond animé
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int gx=0;gx<sw;gx+=sw/24)
            DrawLine(gx,0,gx,sh,{12,12,30,255});
        for(int gy=0;gy<sh;gy+=sh/18)
            DrawLine(0,gy,sw,gy,{12,12,30,255});

        // Titre pulsant
        float pulse=1.f+0.04f*std::sin(_anim*3.f);
        int tsz=(int)(sh/7*pulse);
        const char* title="ZIMEX";
        int tw=MeasureText(title,tsz);
        DrawText(title,sw/2-tw/2+3,sh/9+3,tsz,{80,40,0,180});
        DrawText(title,sw/2-tw/2,  sh/9,  tsz,YELLOW);

        const char* sub="Survival Brotato";
        int ssz=sh/28; int sw2=MeasureText(sub,ssz);
        DrawText(sub,sw/2-sw2/2,(int)(sh*0.27f),ssz,LIGHTGRAY);

        // Boutons
        struct Btn{float ny1,ny2;const char*lbl;Color bg;Color fg;};
        Btn btns[]={
            {0.39f,0.46f,"> Jouer",     {30,90,30,220},GREEN},
            {0.49f,0.56f,"> Paramètres",{20,20,80,220},WHITE},
            {0.59f,0.66f,"> Quitter",   {90,20,20,220},RED},
            {0.69f,0.76f,"> Infos",   {90,20,20,220},YELLOW},  // hover

        };
        for(auto& b:btns){
            int bx=(int)(sw*0.33f),by=(int)(sh*b.ny1);
            int bw=(int)(sw*0.34f),bh=(int)(sh*(b.ny2-b.ny1));
            DrawRectangle(bx,by,bw,bh,b.bg);
            DrawRectangleLines(bx,by,bw,bh,{80,80,80,200});
            int lsz=bh*6/10;
            DrawText(b.lbl,bx+14,by+bh/2-lsz/2,lsz,b.fg);
        }
    }
};




// ================================================================
// INFOS SCENE
// ================================================================
class infoScene : public Scene {
    SceneManager*  _sm;
    GlobalComponent* _g;
    float _brightness=1.f;
public:
    infoScene(SceneManager& sm, GlobalComponent& g):Scene("Infos"),_sm(&sm),_g(&g){}
    void onEnter() override {}
    void onExit()  override {}

    void update(float dt) override {
        (void)dt;
        if(IsKeyPressed(KEY_F11)){_g->isFullscreen=!_g->isFullscreen;ToggleFullscreen();}
        // Retour
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            float ny=GetMousePosition().y/GetScreenHeight();
            if(ny>=0.80f&&ny<=0.90f) _sm->changeScene(SceneType::MENU);
        }
        if(IsKeyPressed(KEY_ESCAPE)) _sm->changeScene(SceneType::MENU);
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        // Overlay luminosité
        if(_brightness<1.f){
            unsigned char a=(unsigned char)((1.f-_brightness)*200.f);
            DrawRectangle(0,0,sw,sh,{0,0,0,a});
        }
        
        // Guide des touches
        int gy=(int)(sh*0.71f);
        int gsz=sh/32;
        int col1=sw/20, col2=(int)(sw*0.35f);
        Color title_c={180,180,255,255};
        Color key_c  ={255,220,50,255};
        Color val_c  ={200,200,200,255};

        DrawText("── CONTRÔLES ──",col1,gy,gsz,title_c);
        DrawText("── ARMES ──",col2,gy,gsz,title_c);
        gy+=gsz+4;

        struct KV{const char*k;const char*v;};
        KV ctrl[]={
            {"WASD / Flèches","Déplacer le joueur"},
            {"Clic Gauche (maintenu)","Tirer"},
            {"Clic Droit / ScrollWheel","Changer d'arme"},
            {"R","Recharger"},
            {"E","Ramasser un pickup"},
            {"F11","Plein écran"},
            {"Echap","Quitter"},
        };
        KV weapons[]={
            {"Pistol","20 dmg  |  12 balles  |  semi-auto"},
            {"Shotgun","8 dmg x7  |  6 balles  |  cône 40°"},
            {"Sniper","60 dmg  |  5 balles  |  perce x3"},
            {"AK-47","12 dmg  |  30 balles  |  rafale auto"},
            {"Bomb","80+50 dmg AoE  |  3 balles  |  explosion"},
        };
        KV drops[]={
            {"Orbe VERT","Ramasser = +matériaux"},
            {"Icône JAUNE","Recharge munitions (toutes armes)"},
            {"Icône VERTE","Soin +50 HP"},
            {"Icône BLEUE","Bouclier magnétique 360 HP"},
        };

        for(auto&kv:ctrl){
            DrawText(kv.k,col1,gy,gsz-2,key_c);
            DrawText(kv.v,col1+MeasureText(kv.k,gsz-2)+8,gy,gsz-2,val_c);
            gy+=gsz+2;
        }
        gy=(int)(sh*0.71f)+gsz+4;
        for(auto&kv:weapons){
            DrawText(kv.k,col2,gy,gsz-2,{255,200,50,255});
            DrawText(kv.v,col2+MeasureText(kv.k,gsz-2)+8,gy,gsz-2,val_c);
            gy+=gsz+2;
        }
        gy+=gsz;
        DrawText("── PICKUPS ──",col2,gy,gsz,title_c);
        gy+=gsz+4;
        for(auto&kv:drops){
            DrawText(kv.k,col2,gy,gsz-2,key_c);
            DrawText(kv.v,col2+MeasureText(kv.k,gsz-2)+8,gy,gsz-2,val_c);
            gy+=gsz+2;
        }
    }
};


// ================================================================
// SETTINGS SCENE
// ================================================================
class SettingsScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _brightness=1.f;
public:
    SettingsScene(SceneManager& sm, GlobalComponent& g):Scene("Settings"),_sm(&sm),_g(&g){}
    void onEnter() override {}
    void onExit()  override {}

    void update(float dt) override {
        (void)dt;
        // Volume
        if(IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD))
            SetMasterVolume(_g->masterVolume=std::min(1.f,_g->masterVolume+0.05f));
        if(IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT))
            SetMasterVolume(_g->masterVolume=std::max(0.f,_g->masterVolume-0.05f));
        // Luminosité (+/- sur flèches)
        if(IsKeyPressed(KEY_UP))   _brightness=std::min(1.f,_brightness+0.05f);
        if(IsKeyPressed(KEY_DOWN)) _brightness=std::max(0.1f,_brightness-0.05f);
        // Fullscreen F11
        if(IsKeyPressed(KEY_F11)){_g->isFullscreen=!_g->isFullscreen;ToggleFullscreen();}
        // Retour
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            float ny=GetMousePosition().y/GetScreenHeight();
            if(ny>=0.80f&&ny<=0.90f) _sm->changeScene(SceneType::MENU);
        }
        if(IsKeyPressed(KEY_ESCAPE)) _sm->changeScene(SceneType::MENU);
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        // Overlay luminosité
        if(_brightness<1.f){
            unsigned char a=(unsigned char)((1.f-_brightness)*200.f);
            DrawRectangle(0,0,sw,sh,{0,0,0,a});
        }

        int tsz=sh/9;
        const char*t="PARAMÈTRES"; int tw=MeasureText(t,tsz);
        DrawText(t,sw/2-tw/2+2,sh/10+2,tsz,{0,0,0,140});
        DrawText(t,sw/2-tw/2,  sh/10,  tsz,WHITE);

        int lsz=sh/20;
        int lx=(int)(sw*0.18f);
        int ly=(int)(sh*0.32f);
        int ls=lsz+lsz/2;

        // Volume
        DrawText("Volume",lx,ly,lsz,WHITE);
        std::string vs=std::to_string((int)(_g->masterVolume*100))+"%";
        int bx=(int)(sw*0.42f),by=ly,bw=(int)(sw*0.40f),bh=lsz;
        DrawRectangle(bx,by,bw,bh,DARKGRAY);
        DrawRectangle(bx,by,(int)(bw*_g->masterVolume),bh,GREEN);
        DrawRectangleLines(bx,by,bw,bh,WHITE);
        DrawText(vs.c_str(),bx+bw+8,by,lsz,GREEN);
        DrawText("+/-",lx+(int)(sw*0.18f),by,lsz-4,GRAY);
        ly+=ls;

        // Luminosité
        DrawText("Luminosité",lx,ly,lsz,WHITE);
        std::string bs=std::to_string((int)(_brightness*100))+"%";
        DrawRectangle(bx,ly,bw,bh,DARKGRAY);
        DrawRectangle(bx,ly,(int)(bw*_brightness),bh,YELLOW);
        DrawRectangleLines(bx,ly,bw,bh,WHITE);
        DrawText(bs.c_str(),bx+bw+8,ly,lsz,YELLOW);
        DrawText("↑/↓",lx+(int)(sw*0.18f),ly,lsz-4,GRAY);
        ly+=ls;

        // Fullscreen
        bool fs=_g->isFullscreen;
        std::string fss=std::string("Plein écran  [F11]  : ")+(fs?"ON":"OFF");
        DrawText(fss.c_str(),lx,ly,lsz,fs?GREEN:GRAY);
        ly+=ls;

        // Infos
        DrawText("F11 = activer/désactiver le plein écran",lx,ly,lsz-4,{120,120,120,255});

        // Bouton retour
        int bky=(int)(sh*0.82f),bkh=(int)(sh*0.08f),bkw=(int)(sw*0.34f);
        int bkx=sw/2-bkw/2;
        DrawRectangle(bkx,bky,bkw,bkh,{20,20,80,220});
        DrawRectangleLines(bkx,bky,bkw,bkh,{80,80,120,255});
        const char*bk="< Retour au Menu";
        int bksz=bkh*6/10;
        DrawText(bk,bkx+bkw/2-MeasureText(bk,bksz)/2,bky+bkh/2-bksz/2,bksz,{100,200,255,255});
    }
};

// ================================================================
// GAME OVER / VICTOIRE
// ================================================================
class GameOverScene : public Scene {
    SceneManager* _sm;
    bool _victory;
    float _timer=0;
public:
    GameOverScene(SceneManager& sm, bool victory=false)
        :Scene(victory?"Victory":"GameOver"),_sm(&sm),_victory(victory){}
    void onEnter() override {_timer=0;}
    void onExit()  override {}
    void update(float dt) override {
        _timer+=dt;
        if(_timer>0.8f&&(IsKeyPressed(KEY_ENTER)||IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            _sm->changeScene(SceneType::MENU);
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        // Effet scan lines
        for(int y=0;y<sh;y+=4) DrawRectangle(0,y,sw,2,{0,0,0,40});

        int tsz=sh/7;
        const char* t=_victory?"VICTOIRE !":"GAME OVER";
        Color tc=_victory?YELLOW:RED;
        // Ombre
        DrawText(t,sw/2-MeasureText(t,tsz)/2+4,sh/3+4,tsz,{0,0,0,200});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/3,  tsz,tc);

        if(_victory){
            const char*s="20 Vagues Complétées !";
            int ssz=sh/16; DrawText(s,sw/2-MeasureText(s,ssz)/2,(int)(sh*0.55f),ssz,GREEN);
        }

        if(_timer>0.8f){
            float alpha=std::min(1.f,(_timer-0.8f)/0.5f);
            const char* sub="Appuie sur ENTRÉE ou clique pour revenir au menu";
            int ssz=sh/26;
            Color sc={255,255,255,(unsigned char)(alpha*255)};
            DrawText(sub,sw/2-MeasureText(sub,ssz)/2,(int)(sh*0.72f),ssz,sc);
        }
    }
};

// ================================================================
// GAME SCENE
// ================================================================
class GameScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;

    RenderSystem    _render;
    InputSystem     _input;
    CollisionSystem _collision;
    PhysicSystem    _physics;
    HealthSystem    _health;
    AnimationSystem _animation;
    WeaponSystem    _weapon;
    EnemySystem     _enemy;
    BulletSystem    _bullet;
    WaveSystem      _wave;
    SoundSystem     _sound;

    InputContext _ctx;
    Entity _player{0};
    Entity _waveEnt{0};

    // HUD cache
    float _hp=200,_maxHp=200;
    float _shieldHp=0,_shieldMax=360;
    bool  _hasShield=false;
    int   _ammo=0,_maxAmmo=0;
    std::string _wname="Pistol";
    std::string _w2name="Shotgun";
    bool  _reloading=false;
    float _reloadPct=0;
    int   _wave_n=1,_mats=0,_kills=0;
    WaveState _wstate=WaveState::FIGHTING;
    float _betweenT=0,_betweenMax=4;

public:
    GameScene(SceneManager& sm, GlobalComponent& g)
        :Scene("Game"),_sm(&sm),_g(&g){}

    // ── Init ────────────────────────────────────────────────────
    void initSystems(){
        auto W=[&](System& s){s.setWorld(world);};
        W(_render);    _render.setGlobal(*_g); _render.initRenderTarget();
        W(_input);
        W(_collision);
        W(_physics);
        W(_health);
        W(_animation); _animation.setGlobal(*_g);
        W(_weapon);    _weapon.setGlobal(*_g);
        W(_enemy);     _enemy.setGlobal(*_g); _enemy.setSceneManager(*_sm);
        W(_bullet);
        W(_wave);      _wave.setGlobal(*_g); _wave.setSceneManager(*_sm);
        W(_sound);     _sound.setGlobal(*_g);

        // WASD mouvement
        _ctx.bind(KEY_W,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->y-=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_S,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->y+=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_A,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->x-=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_D,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->x+=pl->speed*GetFrameTime();
        });
        // Flèches aussi
        _ctx.bind(KEY_UP,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->y-=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_DOWN,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->y+=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_LEFT,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->x-=pl->speed*GetFrameTime();
        });
        _ctx.bind(KEY_RIGHT,[](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl) p->x+=pl->speed*GetFrameTime();
        });
        _input.setInputContext(&_ctx);
    }

    void spawnBackground(){
        // Fond
        Entity bg=world.createEntity();
        PositionComponent p; p.x=0; p.y=0; world.addComponent(bg,p);
        RectangleComponent r; r.width=1920;r.height=1080;
        r.inlineColor={5,5,15,255};r.tickness=0;r.layer=0;
        world.addComponent(bg,r);
        // Grille
        for(int gx=0;gx<1920;gx+=64){
            Entity gl=world.createEntity();
            PositionComponent gp; gp.x=(float)gx;gp.y=0; world.addComponent(gl,gp);
            RectangleComponent gr; gr.width=1;gr.height=1080;
            gr.inlineColor={14,14,32,255};gr.tickness=0;gr.layer=0;
            world.addComponent(gl,gr);
        }
        for(int gy=0;gy<1080;gy+=64){
            Entity gl=world.createEntity();
            PositionComponent gp; gp.x=0;gp.y=(float)gy; world.addComponent(gl,gp);
            RectangleComponent gr; gr.width=1920;gr.height=1;
            gr.inlineColor={14,14,32,255};gr.tickness=0;gr.layer=0;
            world.addComponent(gl,gr);
        }
    }

    void spawnPlayer(){
        _player=world.createEntity();
        PositionComponent pos; pos.x=960;pos.y=540; world.addComponent(_player,pos);

        // 200 HP
        HealthComponent hp; hp.hp=hp.maxHp=200.f; world.addComponent(_player,hp);

        SpriteComponent spr; spr.texturePath=0;
        float tw=32,th=32;
        if((int)_g->_allSprites.size()>0){
            tw=(float)_g->_allSprites[0].width; th=(float)_g->_allSprites[0].height;
        }
        spr.left=0;spr.top=0;spr.width=tw;spr.height=th;spr.scale=2.f;
        spr.offsetX=-tw*spr.scale/2.f; spr.offsetY=-th*spr.scale/2.f;
        spr.tag=EntityTag::PLAYER; spr.layer=2; world.addComponent(_player,spr);

        BoxColliderComponent col; col.isCircle=true; col.radius=18.f;
        col.TagsCollided={(int)EntityTag::BULLET_EN,(int)EntityTag::PICKUP,
                          (int)EntityTag::XP_ORB,  (int)EntityTag::ENEMY,
                          (int)EntityTag::BOSS,    (int)EntityTag::PICKUP_AMMO,
                          (int)EntityTag::PICKUP_HP,(int)EntityTag::PICKUP_SHIELD};
        world.addComponent(_player,col);
        world.addComponent(_player,InputComponent{});

        PlayerComponent pl; pl.speed=200.f; world.addComponent(_player,pl);

        // Armes de départ : slot0=Pistol, slot1=Shotgun
        WeaponComponent wc;
        wc.slots[0]={createWeapon("pistol",&world),"pistol"};
        wc.slots[1]={createWeapon("shotgun",&world),"shotgun"};
        wc.activeSlot=0;
        world.addComponent(_player,wc);
    }

    void spawnWave(){
        _waveEnt=world.createEntity();
        PositionComponent p; world.addComponent(_waveEnt,p);
        WaveComponent wc; wc.currentWave=1;wc.state=WaveState::FIGHTING;
        wc.enemiesLeft=10;wc.spawnRate=1.2f;wc.betweenMax=4.f;
        world.addComponent(_waveEnt,wc);
    }

    void spawnPickupWeapon(float x,float y,const std::string& type,int tex){
        Entity e=world.createEntity();
        PositionComponent p; p.x=x;p.y=y; world.addComponent(e,p);
        float tw=24,th=24;
        if((int)_g->_allSprites.size()>tex){tw=(float)_g->_allSprites[tex].width;th=(float)_g->_allSprites[tex].height;}
        SpriteComponent s; s.texturePath=tex; s.width=tw;s.height=th; s.scale=2.f;
        s.offsetX=-tw*s.scale/2.f; s.offsetY=-th*s.scale/2.f;
        s.tag=EntityTag::PICKUP; s.layer=1; world.addComponent(e,s);
        BoxColliderComponent c; c.isCircle=true; c.radius=22.f;
        c.TagsCollided={(int)EntityTag::PLAYER}; world.addComponent(e,c);
        WeaponComponent wc; wc.isPickup=true; wc.pickupWeaponType=type;
        world.addComponent(e,wc);
    }

    void onEnter() override {
        initSystems();
        spawnBackground();
        spawnPlayer();
        spawnWave();
        // Pickups d'armes initiaux sur la carte
        spawnPickupWeapon(350,250,"sniper",7);
        spawnPickupWeapon(1570,830,"bomb",  8);
        spawnPickupWeapon(1570,250,"ak47",  6);
    }
    void onExit() override { world.clearWorld(); _ctx.clear(); }

    // ── Update ──────────────────────────────────────────────────
    void update(float dt) override {
        // Aim
        Vector2 m=GetMousePosition();
        float vx=m.x*(1920.f/GetScreenWidth());
        float vy=m.y*(1080.f/GetScreenHeight());
        if(auto*pos=world.getComponent<PositionComponent>(_player))
        if(auto*spr=world.getComponent<SpriteComponent>(_player))
            spr->rotation=std::atan2(vy-pos->y,vx-pos->x)*180.f/3.14159f;

        // Tir (maintenu pour AK47)
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            auto*pos=world.getComponent<PositionComponent>(_player);
            auto*wc =world.getComponent<WeaponComponent>(_player);
            if(pos&&wc&&wc->current()) wc->current()->fire(pos->x,pos->y,vx,vy);
        }
        // Swap clic droit ou scroll
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)||GetMouseWheelMove()!=0){
            auto*wc=world.getComponent<WeaponComponent>(_player);
            if(wc) wc->swap();
        }
        // Reload R
        if(IsKeyPressed(KEY_R)){
            auto*wc=world.getComponent<WeaponComponent>(_player);
            if(wc&&wc->current()) wc->current()->reload();
        }
        // Pickup E (armes)
        if(IsKeyPressed(KEY_E)) doPickupWeapon();

        // Echap = pause / retour menu
        if(IsKeyPressed(KEY_ESCAPE)) { _sm->changeScene(SceneType::MENU); return; }

        _input.update(dt);

        // Clamp joueur
        if(auto*p=world.getComponent<PositionComponent>(_player)){
            p->x=std::max(30.f,std::min(p->x,1890.f));
            p->y=std::max(30.f,std::min(p->y,1050.f));
        }

        _collision.update(dt);
        _bullet.update(dt);
        _enemy.update(dt);
        _wave.update(dt);
        _weapon.update(dt);
        _sound.update(dt);
        _animation.update(dt);

        handlePlayerDamage(dt);
        handleInvinc(dt);
        collectPickups();
        checkDeath();
        syncHUD();
    }

    // Ramasser une arme
    void doPickupWeapon(){
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        if(!col) return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);
            if(!spr||spr->tag!=EntityTag::PICKUP) continue;
            auto*pwc=world.getComponent<WeaponComponent>(o);
            if(!pwc||!pwc->isPickup) continue;
            auto*plwc=world.getComponent<WeaponComponent>(_player);
            if(plwc){
                // Mettre dans le slot secondaire
                int slot=1-plwc->activeSlot;
                plwc->slots[slot]={createWeapon(pwc->pickupWeaponType,&world),pwc->pickupWeaponType};
            }
            rm.push_back(o);
        }
        for(Entity e:rm) world.removeEntity(e);
    }

    // Collecte automatique pickups HP/ammo/bouclier/xp
    void collectPickups(){
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        auto*pl =world.getComponent<PlayerComponent>(_player);
        auto*hp =world.getComponent<HealthComponent>(_player);
        auto*wc =world.getComponent<WeaponComponent>(_player);
        if(!col||!pl||!hp) return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);
            if(!spr) continue;

            if(spr->tag==EntityTag::XP_ORB){ pl->materials++; rm.push_back(o); }

            else if(spr->tag==EntityTag::PICKUP_AMMO){
                // Recharge les 2 armes
                if(wc){ for(auto& s:wc->slots) if(s.weapon) s.weapon->refillAmmo(); }
                rm.push_back(o);
            }
            else if(spr->tag==EntityTag::PICKUP_HP){
                hp->hp=std::min(hp->maxHp, hp->hp+50.f);
                rm.push_back(o);
            }
            else if(spr->tag==EntityTag::PICKUP_SHIELD){
                pl->hasShield=true;
                pl->shieldHp=pl->shieldMax;
                rm.push_back(o);
            }
        }
        for(Entity e:rm) world.removeEntity(e);
    }

    void handlePlayerDamage(float dt){
        auto*pl=world.getComponent<PlayerComponent>(_player);
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        auto*hp=world.getComponent<HealthComponent>(_player);
        if(!pl||!col||!hp||pl->isInvincible) return;

        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o); if(!spr) continue;
            bool hit=false;
            float dmg=0;

            if(spr->tag==EntityTag::BULLET_EN){
                auto*b=world.getComponent<BulletComponent>(o);
                if(b) dmg=b->damage;
                world.removeEntity(o); hit=true;
            } else if(spr->tag==EntityTag::ENEMY||spr->tag==EntityTag::BOSS){
                auto*ai=world.getComponent<EnemyAIComponent>(o);
                dmg=(ai?ai->damage:8.f)*dt*2.f; hit=true;
            }

            if(hit&&dmg>0){
                // Bouclier absorbe en premier
                if(pl->hasShield&&pl->shieldHp>0){
                    pl->shieldHp-=dmg;
                    if(pl->shieldHp<=0){pl->shieldHp=0;pl->hasShield=false;}
                } else {
                    hp->hp-=dmg;
                }
                pl->isInvincible=true; pl->invincTimer=0.6f;
                if(spr->tag==EntityTag::BULLET_EN) break;
            }
        }
    }

    void handleInvinc(float dt){
        auto*pl=world.getComponent<PlayerComponent>(_player); if(!pl) return;
        if(!pl->isInvincible) return;
        pl->invincTimer-=dt;
        if(pl->invincTimer<=0.f){pl->invincTimer=0;pl->isInvincible=false;}
        if(auto*spr=world.getComponent<SpriteComponent>(_player))
            spr->tint=(pl->isInvincible&&(int)(pl->invincTimer*12)%2==0)
                     ?Color{255,80,80,160}:WHITE;
    }

    void checkDeath(){
        auto*hp=world.getComponent<HealthComponent>(_player);
        if(hp&&hp->hp<=0.f){ _sm->changeScene(SceneType::GAMEOVER); return; }
        auto*wv=world.getComponent<WaveComponent>(_waveEnt);
        if(wv&&wv->state==WaveState::VICTORY){ _sm->changeScene(SceneType::GAMEOVER); return; }
    }

    void syncHUD(){
        if(auto*hp=world.getComponent<HealthComponent>(_player))
            {_hp=hp->hp;_maxHp=hp->maxHp;}
        if(auto*pl=world.getComponent<PlayerComponent>(_player))
            {_mats=pl->materials;_kills=pl->kills;
             _hasShield=pl->hasShield;_shieldHp=pl->shieldHp;_shieldMax=pl->shieldMax;}
        if(auto*wc=world.getComponent<WeaponComponent>(_player)){
            _ammo=wc->ammo();_maxAmmo=wc->maxAmmo();
            _wname=wc->weaponName(); _reloading=wc->reloading();
            int other=1-wc->activeSlot;
            _w2name=wc->slots[other].weapon?wc->slots[other].weapon->getName():"--";
        }
        if(auto*wv=world.getComponent<WaveComponent>(_waveEnt))
            {_wave_n=wv->currentWave;_wstate=wv->state;
             _betweenT=wv->betweenTimer;_betweenMax=wv->betweenMax;}
    }

    // ── Render ──────────────────────────────────────────────────
    void render(float dt) override {
        _render.update(dt);
        drawEnemyHP();
        drawHUD();
    }

    // Barres de HP au-dessus de chaque ennemi
    void drawEnemyHP(){
        float scaleX=(float)GetScreenWidth()/1920.f;
        float scaleY=(float)GetScreenHeight()/1080.f;
        float sc=std::min(scaleX,scaleY);
        float ox=(GetScreenWidth() -1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;

        for(Entity e:world.getContainer<EnemyAIComponent>().getEntities()){
            auto*pos=world.getComponent<PositionComponent>(e);
            auto*hp =world.getComponent<HealthComponent>(e);
            auto*ai =world.getComponent<EnemyAIComponent>(e);
            auto*spr=world.getComponent<SpriteComponent>(e);
            if(!pos||!hp||!ai) continue;

            float ratio=hp->maxHp>0?hp->hp/hp->maxHp:0;
            float barW=ai->isBoss?90.f:50.f;
            float barH=ai->isBoss?12.f:7.f;
            float sprH=(spr?spr->height*spr->scale:32.f);

            // Coords virtuelles -> coords écran
            float sx=(float)(ox+(pos->x-barW/2)*sc);
            float sy=(float)(oy+(pos->y-sprH/2-barH-6)*sc);
            float sw=barW*sc, sh=barH*sc;

            DrawRectangle((int)sx,(int)sy,(int)sw,(int)sh,DARKGRAY);
            Color hcol=ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED;
            if(ai->isBoss) hcol=ratio>0.5f?ORANGE:RED;
            DrawRectangle((int)sx,(int)sy,(int)(sw*ratio),(int)sh,hcol);
            DrawRectangleLines((int)sx,(int)sy,(int)sw,(int)sh,WHITE);

            // Nom + HP pour boss
            if(ai->isBoss){
                std::string bstr="BOSS  "+std::to_string((int)hp->hp)+"/"+std::to_string((int)hp->maxHp);
                int bsz=(int)(12*sc);
                DrawText(bstr.c_str(),(int)sx,(int)(sy-bsz-2),bsz,ORANGE);
            }
        }
    }

    void drawHUD(){
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        int hudH=54;
        DrawRectangle(0,sh-hudH,sw,hudH,{0,0,0,225});
        DrawLine(0,sh-hudH,sw,sh-hudH,{50,50,70,255});

        int y=sh-hudH+8;
        int sz=20;

        // ── HP bar ──────────────────────────────────────────
        float ratio=_maxHp>0?_hp/_maxHp:0;
        DrawRectangle(8,y,220,32,DARKGRAY);
        Color hc=ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED;
        DrawRectangle(8,y,(int)(220*ratio),32,hc);
        DrawRectangleLines(8,y,220,32,WHITE);
        DrawText(TextFormat("HP %.0f/%.0f",_hp,_maxHp),14,y+6,sz,WHITE);

        // ── Bouclier ────────────────────────────────────────
        if(_hasShield){
            float sr=_shieldMax>0?_shieldHp/_shieldMax:0;
            DrawRectangle(8,y-22,220,14,DARKGRAY);
            DrawRectangle(8,y-22,(int)(220*sr),14,{60,120,255,255});
            DrawRectangleLines(8,y-22,220,14,{100,180,255,255});
            DrawText(TextFormat("SHIELD %.0f",_shieldHp),14,y-20,12,{150,200,255,255});
        }

        DrawLine(236,sh-hudH,236,sh,{50,50,70,255});

        // ── Arme active ─────────────────────────────────────
        int ax=244;
        Color wc_col=_reloading?ORANGE:YELLOW;
        DrawText(TextFormat("[%s]",_wname.c_str()),ax,y,sz,wc_col);
        int ww=MeasureText(TextFormat("[%s]",_wname.c_str()),sz);
        DrawText(TextFormat("%d/%d",_ammo,_maxAmmo),ax+ww+6,y,sz,WHITE);
        if(_reloading)
            DrawText("RECHARGEMENT...",ax,y-18,14,ORANGE);

        // Arme secondaire en gris
        DrawText(TextFormat("2e:[%s]",_w2name.c_str()),ax,(int)(y+sz+2),14,GRAY);

        DrawLine(ax+190,sh-hudH,ax+190,sh,{50,50,70,255});

        // ── Vague ───────────────────────────────────────────
        int vx=ax+198;
        Color wv_col=_wstate==WaveState::BETWEEN_WAVES?ORANGE:WHITE;
        DrawText(TextFormat("Vague %d/20",_wave_n),vx,y,sz,wv_col);
        if(_wstate==WaveState::BETWEEN_WAVES){
            std::string ct=TextFormat("Prochaine dans %.0fs",_betweenT);
            DrawText(ct.c_str(),vx,y-18,14,ORANGE);
        }
        DrawLine(vx+160,sh-hudH,vx+160,sh,{50,50,70,255});

        // ── Stats ───────────────────────────────────────────
        int sx2=vx+168;
        DrawText(TextFormat("Mat: %d",_mats),sx2,y,sz,GREEN);
        DrawText(TextFormat("Kills: %d",_kills),sx2+100,y,sz,ORANGE);

        // ── Aide ────────────────────────────────────────────
        const char* help="[R]Reload [E]Pickup [ScrollWheel/Clic D]Swap [Echap]Menu";
        int fsz=14;
        DrawText(help,sw-MeasureText(help,fsz)-8,sh-fsz-6,fsz,{90,90,90,255});

        // ── Crosshair ───────────────────────────────────────
        Vector2 m=GetMousePosition();
        int cx=(int)m.x,cy=(int)m.y;
        DrawLine(cx-14,cy,cx-5,cy,{255,255,255,200});
        DrawLine(cx+5, cy,cx+14,cy,{255,255,255,200});
        DrawLine(cx,cy-14,cx,cy-5, {255,255,255,200});
        DrawLine(cx,cy+5, cx,cy+14,{255,255,255,200});
        DrawCircleLines(cx,cy,5,{255,255,255,160});
    }
};
