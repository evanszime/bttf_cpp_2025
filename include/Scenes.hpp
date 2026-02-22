#pragma once
#include "Engine.hpp"
#include "IGame.hpp"
#include <memory>
#include <cmath>
#include <string>

std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

// ================================================================
// INFO SCENE - Guide complet du jeu
// ================================================================
class InfoScene : public Scene {
    SceneManager* _sm;
    int _page=0;
    static const int PAGES=2;
public:
    InfoScene(SceneManager& sm):Scene("Info"),_sm(&sm){}
    void onEnter() override {_page=0;}
    void onExit()  override {}
    void update(float dt) override {
        (void)dt;
        if(IsKeyPressed(KEY_RIGHT)||IsKeyPressed(KEY_D)) _page=(_page+1)%PAGES;
        if(IsKeyPressed(KEY_LEFT) ||IsKeyPressed(KEY_A)) _page=(_page-1+PAGES)%PAGES;
        if(IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_BACKSPACE)) _sm->changeScene(SceneType::MENU);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            float nx=GetMousePosition().x/GetScreenWidth();
            float ny=GetMousePosition().y/GetScreenHeight();
            if(ny>0.88f){
                if(nx<0.4f) _page=(_page-1+PAGES)%PAGES;
                else if(nx>0.6f) _page=(_page+1)%PAGES;
                else _sm->changeScene(SceneType::MENU);
            }
        }
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int gy=0;gy<sh;gy+=sh/20) DrawLine(0,gy,sw,gy,{10,10,28,255});
        for(int gx=0;gx<sw;gx+=sw/24) DrawLine(gx,0,gx,sh,{10,10,28,255});

        int tsz=sh/14;
        const char* t="COMMENT JOUER";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+2,sh/16+2,tsz,{0,0,0,140});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/16,  tsz,YELLOW);

        int lsz=sh/26;
        int lx=sw/16, ly=(int)(sh*0.18f), ls=(int)(lsz*1.6f);
        Color key_c={255,220,50,255}, val_c={200,200,200,255};
        Color sec_c={100,200,255,255};

        if(_page==0){
            // Page 1 : Contrôles + But du jeu
            DrawText("── BUT DU JEU ──",lx,ly,lsz,sec_c); ly+=ls;
            DrawText("Survivre 20 vagues de monstres qui arrivent de tous côtés !",lx,ly,lsz-2,val_c); ly+=ls;
            DrawText("Chaque vague se termine par un BOSS. Ramasse des pickups pour survivre.",lx,ly,lsz-2,val_c); ly+=ls*2;

            DrawText("── MOUVEMENT & TIR ──",lx,ly,lsz,sec_c); ly+=ls;
            struct KV{const char*k;const char*v;};
            KV ctrl[]={
                {"WASD / Flèches","Déplacer le joueur"},
                {"Clic Gauche (maintenu)","Tirer vers la souris"},
                {"WASD + Clic G","La direction WASD oriente aussi les tirs"},
                {"Clic Droit","Changer d'arme (slot 2)"},
                {"Molette Souris","Changer d'arme"},
                {"R","Recharger l'arme active"},
                {"E","Ramasser un pickup d'arme"},
                {"Echap","Retourner au menu"},
            };
            for(auto& kv:ctrl){
                DrawText(kv.k,lx,ly,lsz-2,key_c);
                DrawText(kv.v,lx+MeasureText(kv.k,lsz-2)+12,ly,lsz-2,val_c);
                ly+=ls;
            }

            ly+=ls/2;
            DrawText("── INTERFACE ──",lx,ly,lsz,sec_c); ly+=ls;
            KV ui[]={
                {"F11","Plein écran (redimension auto)"},
                {"+ / -","Augmenter / Diminuer le volume"},
                {"Flèche Haut/Bas","Luminosité (dans Paramètres)"},
            };
            for(auto& kv:ui){
                DrawText(kv.k,lx,ly,lsz-2,key_c);
                DrawText(kv.v,lx+MeasureText(kv.k,lsz-2)+12,ly,lsz-2,val_c);
                ly+=ls;
            }
        } else {
            // Page 2 : Armes + Pickups + Ennemis
            int col1=sw/16, col2=sw/2+sw/16;
            int cy=ly;

            // Colonne gauche : Armes
            DrawText("── ARMES ──",col1,cy,lsz,sec_c); cy+=ls;
            struct W{const char*name;Color col;const char*desc;};
            W weapons[]={
                {"Pistol",      YELLOW,  "20 dmg | 120 balles | semi-auto"},
                {"Shotgun",     ORANGE,  "8 dmg x7 | 60 balles | cône 40°"},
                {"Sniper",      SKYBLUE, "60 dmg | 50 balles | perce x3"},
                {"AK-47",       GREEN,   "12 dmg | 300 balles | rafale auto"},
                {"Bomb",        RED,     "80+50 AoE | 30 balles | explosion"},
            };
            for(auto& w:weapons){
                DrawText(w.name,col1,cy,lsz-2,w.col);
                DrawText(w.desc,col1+MeasureText(w.name,lsz-2)+10,cy,lsz-2,val_c);
                cy+=ls;
            }
            cy+=ls/2;
            DrawText("── PICKUPS (drop des ennemis) ──",col1,cy,lsz,sec_c); cy+=ls;
            struct P{const char*k;Color c;const char*v;};
            P picks[]={
                {"Orbe VERT",       GREEN,                    "Matériaux +1"},
                {"Icône JAUNE",     YELLOW,                   "Recharge munitions"},
                {"Icône VERTE",     {50,255,100,255},         "Soin +50 HP"},
                {"Icône BLEUE",     {80,150,255,255},         "Bouclier 360 HP (cercle bleu)"},
                {"Icône ARME",      WHITE,                    "Nouvelle arme (ramasser avec E)"},
            };
            for(auto& p:picks){
                DrawText(p.k,col1,cy,lsz-2,p.c);
                DrawText(p.v,col1+MeasureText(p.k,lsz-2)+10,cy,lsz-2,val_c);
                cy+=ls;
            }

            // Colonne droite : Ennemis + Boss
            cy=ly;
            DrawText("── ENNEMIS ──",col2,cy,lsz,sec_c); cy+=ls;
            DrawText("• 30 sprites différents de Brotato",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• HP: 60 | Vitesse & dégâts croissants",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• Vague N = 10×N ennemis",col2,cy,lsz-2,val_c); cy+=ls;
            cy+=ls/2;
            DrawText("── BOSS (fin de chaque vague) ──",col2,cy,lsz,{255,100,100,255}); cy+=ls;
            DrawText("• 7 types de boss Brotato",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• HP: 400 + vague×80",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• Phase 2 à 50% HP : +vitesse +dégâts",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• Tir en rafale (burst x3→x5)",col2,cy,lsz-2,val_c); cy+=ls;
            cy+=ls/2;
            DrawText("── BOUCLIER ──",col2,cy,lsz,{80,150,255,255}); cy+=ls;
            DrawText("• Cercle bleu visible autour du joueur",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• 360 HP | Absorbe les dégâts en premier",col2,cy,lsz-2,val_c); cy+=ls;
            DrawText("• Drop des ennemis (15% de chance)",col2,cy,lsz-2,val_c); cy+=ls;
        }

        // Navigation
        int nsz=sh/24;
        DrawLine(0,sh-50,sw,sh-50,{40,40,60,255});
        if(_page>0)     DrawText("< Précédent",sw/16,sh-40,nsz,{100,200,255,255});
        if(_page<PAGES-1) DrawText("Suivant >",sw-MeasureText("Suivant >",nsz)-sw/16,sh-40,nsz,{100,200,255,255});
        const char* retour="[ Retour Menu ]";
        DrawText(retour,sw/2-MeasureText(retour,nsz)/2,sh-40,nsz,GRAY);
        // Indicateur page
        std::string pg=std::to_string(_page+1)+"/"+std::to_string(PAGES);
        DrawText(pg.c_str(),sw-MeasureText(pg.c_str(),nsz)-10,10,nsz,DARKGRAY);
    }
};

// ================================================================
// MENU SCENE
// ================================================================
class MenuScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _anim=0;
public:
    MenuScene(SceneManager& sm,GlobalComponent& g):Scene("Menu"),_sm(&sm),_g(&g){}
    void onEnter() override {_anim=0;}
    void onExit()  override {}

    void update(float dt) override {
        _anim+=dt;
        if(!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
        Vector2 m=GetMousePosition();
        float nx=m.x/GetScreenWidth(), ny=m.y/GetScreenHeight();
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.36f&&ny<=0.43f) _sm->changeScene(SceneType::GAME);
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.46f&&ny<=0.53f) _sm->changeScene(SceneType::INFO);
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.56f&&ny<=0.63f) _sm->changeScene(SceneType::SETTINGS);
        if(nx>=0.33f&&nx<=0.67f&&ny>=0.66f&&ny<=0.73f) CloseWindow();
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int gx=0;gx<sw;gx+=sw/24) DrawLine(gx,0,gx,sh,{10,10,28,255});
        for(int gy=0;gy<sh;gy+=sh/18) DrawLine(0,gy,sw,gy,{10,10,28,255});

        float pulse=1.f+0.04f*std::sin(_anim*3.f);
        int tsz=(int)(sh/7*pulse);
        const char* title="ZIMEX";
        DrawText(title,sw/2-MeasureText(title,tsz)/2+3,sh/10+3,tsz,{80,40,0,160});
        DrawText(title,sw/2-MeasureText(title,tsz)/2,  sh/10,  tsz,YELLOW);

        const char* sub="Survive 20 Waves of Monsters !";
        int ssz=sh/28;
        DrawText(sub,sw/2-MeasureText(sub,ssz)/2,(int)(sh*0.25f),ssz,LIGHTGRAY);

        struct Btn{float ny1,ny2;const char*lbl;Color bg;Color fg;};
        Btn btns[]={
            {0.36f,0.43f,"> Jouer",      {30,100,30,220},GREEN},
            {0.46f,0.53f,"> Infos / Aide",{20,60,100,220},{100,200,255,255}},
            {0.56f,0.63f,"> Paramètres", {20,20,80,220},WHITE},
            {0.66f,0.73f,"> Quitter",    {90,20,20,220},RED},
        };
        for(auto& b:btns){
            int bx=(int)(sw*0.33f),by=(int)(sh*b.ny1);
            int bw=(int)(sw*0.34f),bh=(int)(sh*(b.ny2-b.ny1));
            DrawRectangle(bx,by,bw,bh,b.bg);
            DrawRectangleLines(bx,by,bw,bh,{80,80,100,200});
            int lsz=bh*6/10;
            DrawText(b.lbl,bx+14,by+bh/2-lsz/2,lsz,b.fg);
        }

        // Mini-guide
        const char* hint="WASD: Bouger | Clic G: Tirer | Scroll: Changer arme | R: Recharger | E: Pickup";
        int fsz=sh/36;
        DrawText(hint,sw/2-MeasureText(hint,fsz)/2,sh-fsz*2,fsz,GRAY);
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
    SettingsScene(SceneManager& sm,GlobalComponent& g):Scene("Settings"),_sm(&sm),_g(&g){}
    void onEnter() override {}
    void onExit()  override {}

    void update(float dt) override {
        (void)dt;
        if(IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD))
            SetMasterVolume(_g->masterVolume=std::min(1.f,_g->masterVolume+0.05f));
        if(IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT))
            SetMasterVolume(_g->masterVolume=std::max(0.f,_g->masterVolume-0.05f));
        if(IsKeyPressed(KEY_UP))   _brightness=std::min(1.f,_brightness+0.05f);
        if(IsKeyPressed(KEY_DOWN)) _brightness=std::max(0.1f,_brightness-0.05f);
        if(IsKeyPressed(KEY_F11)){
            _g->isFullscreen=!_g->isFullscreen;
            if(_g->isFullscreen){
                int mon=GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(mon),GetMonitorHeight(mon));
                ToggleFullscreen();
            } else {
                ToggleFullscreen();
                SetWindowSize(1280,720);
            }
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            float ny=GetMousePosition().y/GetScreenHeight();
            if(ny>=0.82f&&ny<=0.90f) _sm->changeScene(SceneType::MENU);
        }
        if(IsKeyPressed(KEY_ESCAPE)) _sm->changeScene(SceneType::MENU);
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        if(_brightness<1.f){
            unsigned char a=(unsigned char)((1.f-_brightness)*200.f);
            DrawRectangle(0,0,sw,sh,{0,0,0,a});
        }

        int tsz=sh/9;
        const char* t="PARAMÈTRES";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+2,sh/12+2,tsz,{0,0,0,120});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/12,  tsz,WHITE);

        int lsz=sh/20, lx=(int)(sw*0.14f), ly=(int)(sh*0.30f), ls=lsz+lsz/2;
        int bx=(int)(sw*0.44f), bw=(int)(sw*0.42f), bh=lsz;

        DrawText("Volume (+/-) :",lx,ly,lsz,WHITE);
        DrawRectangle(bx,ly,bw,bh,DARKGRAY);
        DrawRectangle(bx,ly,(int)(bw*_g->masterVolume),bh,GREEN);
        DrawRectangleLines(bx,ly,bw,bh,WHITE);
        DrawText(TextFormat("%d%%",(int)(_g->masterVolume*100)),bx+bw+8,ly,lsz,GREEN);
        ly+=ls;

        DrawText("Luminosité (↑/↓) :",lx,ly,lsz,WHITE);
        DrawRectangle(bx,ly,bw,bh,DARKGRAY);
        DrawRectangle(bx,ly,(int)(bw*_brightness),bh,YELLOW);
        DrawRectangleLines(bx,ly,bw,bh,WHITE);
        DrawText(TextFormat("%d%%",(int)(_brightness*100)),bx+bw+8,ly,lsz,YELLOW);
        ly+=ls;

        bool fs=_g->isFullscreen;
        DrawText(TextFormat("Plein écran [F11] : %s",fs?"ON":"OFF"),lx,ly,lsz,fs?GREEN:GRAY);
        ly+=ls;
        DrawText("F11 = bascule plein écran (résolution s'adapte automatiquement)",lx,ly,lsz-4,DARKGRAY);

        int bky=(int)(sh*0.83f),bkh=(int)(sh*0.08f),bkw=(int)(sw*0.34f),bkx=sw/2-bkw/2;
        DrawRectangle(bkx,bky,bkw,bkh,{20,20,80,220});
        DrawRectangleLines(bkx,bky,bkw,bkh,{60,60,120,255});
        const char*bk="< Retour au Menu";
        int bksz=bkh*6/10;
        DrawText(bk,bkx+bkw/2-MeasureText(bk,bksz)/2,bky+bkh/2-bksz/2,bksz,{100,200,255,255});
    }
};

// ================================================================
// GAME OVER
// ================================================================
class GameOverScene : public Scene {
    SceneManager* _sm;
    bool _victory;
    float _timer=0;
public:
    GameOverScene(SceneManager& sm,bool victory=false)
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
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int y=0;y<sh;y+=4) DrawRectangle(0,y,sw,2,{0,0,0,40});
        int tsz=sh/7;
        const char* t=_victory?"VICTOIRE !":"GAME OVER";
        Color tc=_victory?YELLOW:RED;
        DrawText(t,sw/2-MeasureText(t,tsz)/2+4,sh/3+4,tsz,{0,0,0,200});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/3,  tsz,tc);
        if(_victory){ const char*s="20 Vagues Complétées !";int ssz=sh/16;DrawText(s,sw/2-MeasureText(s,ssz)/2,(int)(sh*0.55f),ssz,GREEN); }
        if(_timer>0.8f){
            float alpha=std::min(1.f,(_timer-0.8f)/0.5f);
            const char*sub="Appuie sur ENTRÉE ou clique pour revenir au menu";
            int ssz=sh/26;
            DrawText(sub,sw/2-MeasureText(sub,ssz)/2,(int)(sh*0.72f),ssz,{255,255,255,(unsigned char)(alpha*255)});
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

    // Direction de déplacement pour orienter les tirs
    float _moveX=0,_moveY=0;

    // Animation player
    float _animTimer=0;
    int   _animFrame=0;
    static const int ANIM_FRAMES=2;
    float ANIM_FPS=8.f;

    // HUD
    float _hp=200,_maxHp=200;
    float _shieldHp=0,_shieldMax=360;
    bool  _hasShield=false;
    int   _ammo=0,_maxAmmo=0;
    std::string _wname="Pistol",_w2name="Shotgun";
    bool  _reloading=false;
    int   _wave_n=1,_mats=0,_kills=0;
    WaveState _wstate=WaveState::FIGHTING;
    float _betweenT=0;

public:
    GameScene(SceneManager& sm,GlobalComponent& g):Scene("Game"),_sm(&sm),_g(&g){}

    // ── Init ──────────────────────────────────────────────────
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

        // Mouvement WASD - on enregistre aussi la direction
        _ctx.bind(KEY_W,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->y-=pl->speed*GetFrameTime(); _moveY=-1;}
        });
        _ctx.bind(KEY_S,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->y+=pl->speed*GetFrameTime(); _moveY=1;}
        });
        _ctx.bind(KEY_A,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->x-=pl->speed*GetFrameTime(); _moveX=-1;}
        });
        _ctx.bind(KEY_D,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->x+=pl->speed*GetFrameTime(); _moveX=1;}
        });
        _ctx.bind(KEY_UP,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->y-=pl->speed*GetFrameTime(); _moveY=-1;}
        });
        _ctx.bind(KEY_DOWN,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->y+=pl->speed*GetFrameTime(); _moveY=1;}
        });
        _ctx.bind(KEY_LEFT,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->x-=pl->speed*GetFrameTime(); _moveX=-1;}
        });
        _ctx.bind(KEY_RIGHT,[this](World& w,Entity e){
            auto*p=w.getComponent<PositionComponent>(e);
            auto*pl=w.getComponent<PlayerComponent>(e);
            if(p&&pl){p->x+=pl->speed*GetFrameTime(); _moveX=1;}
        });
        _input.setInputContext(&_ctx);
    }

    void spawnBackground(){
        Entity bg=world.createEntity();
        PositionComponent p; p.x=0;p.y=0; world.addComponent(bg,p);
        RectangleComponent r; r.width=1920;r.height=1080;r.inlineColor={5,5,15,255};r.tickness=0;r.layer=0;
        world.addComponent(bg,r);
        for(int gx=0;gx<1920;gx+=64){
            Entity gl=world.createEntity();
            PositionComponent gp; gp.x=(float)gx;gp.y=0; world.addComponent(gl,gp);
            RectangleComponent gr; gr.width=1;gr.height=1080;gr.inlineColor={13,13,30,255};gr.tickness=0;gr.layer=0;
            world.addComponent(gl,gr);
        }
        for(int gy=0;gy<1080;gy+=64){
            Entity gl=world.createEntity();
            PositionComponent gp; gp.x=0;gp.y=(float)gy; world.addComponent(gl,gp);
            RectangleComponent gr; gr.width=1920;gr.height=1;gr.inlineColor={13,13,30,255};gr.tickness=0;gr.layer=0;
            world.addComponent(gl,gr);
        }
    }

    void spawnPlayer(){
        _player=world.createEntity();
        PositionComponent pos; pos.x=960;pos.y=540; world.addComponent(_player,pos);
        HealthComponent hp; hp.hp=hp.maxHp=200.f; world.addComponent(_player,hp);

        // Utiliser player_sheet pour l'animation
        int texIdx = ((int)_g->_allSprites.size()>27) ? 27 : 0;
        SpriteComponent spr; spr.texturePath=texIdx;
        // Frame 0 : left=0, width=64
        spr.left=0; spr.top=0; spr.width=64; spr.height=64;
        spr.scale=1.8f; spr.offsetX=-64*spr.scale/2.f; spr.offsetY=-64*spr.scale/2.f;
        spr.tag=EntityTag::PLAYER; spr.layer=2;
        world.addComponent(_player,spr);

        BoxColliderComponent col; col.isCircle=true; col.radius=18.f;
        col.TagsCollided={(int)EntityTag::BULLET_EN,(int)EntityTag::PICKUP,
                          (int)EntityTag::XP_ORB,  (int)EntityTag::ENEMY,
                          (int)EntityTag::BOSS,    (int)EntityTag::PICKUP_AMMO,
                          (int)EntityTag::PICKUP_HP,(int)EntityTag::PICKUP_SHIELD};
        world.addComponent(_player,col);
        world.addComponent(_player,InputComponent{});
        PlayerComponent pl; pl.speed=210.f; world.addComponent(_player,pl);

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
        BoxColliderComponent c; c.isCircle=true; c.radius=24.f;
        c.TagsCollided={(int)EntityTag::PLAYER}; world.addComponent(e,c);
        WeaponComponent wc; wc.isPickup=true; wc.pickupWeaponType=type;
        world.addComponent(e,wc);
    }

    void onEnter() override {
        _moveX=0;_moveY=0;_animTimer=0;_animFrame=0;
        initSystems();
        spawnBackground();
        spawnPlayer();
        spawnWave();
        spawnPickupWeapon(350,250,"sniper",7);
        spawnPickupWeapon(1570,830,"bomb",  8);
        spawnPickupWeapon(1570,250,"ak47",  6);
    }
    void onExit() override { world.clearWorld(); _ctx.clear(); }

    // ── Update ─────────────────────────────────────────────────
    void update(float dt) override {
        _moveX=0;_moveY=0;

        // Echap
        if(IsKeyPressed(KEY_ESCAPE)){_sm->changeScene(SceneType::MENU);return;}

        // Mouvement (enregistre direction)
        _input.update(dt);

        // Clamp joueur
        if(auto*p=world.getComponent<PositionComponent>(_player)){
            p->x=std::max(30.f,std::min(p->x,1890.f));
            p->y=std::max(30.f,std::min(p->y,1050.f));
        }

        // ── Tir automatique ──────────────────────────────
        // Direction = vers la souris OU vers la direction WASD si en mouvement
        auto*playerPos=world.getComponent<PositionComponent>(_player);
        if(playerPos){
            float vx,vy;
            Vector2 m=GetMousePosition();
            float mx=m.x*(1920.f/GetScreenWidth());
            float my=m.y*(1080.f/GetScreenHeight());

            // Priorité: souris si clic maintenu, sinon joystick WASD
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                vx=mx-playerPos->x; vy=my-playerPos->y;
            } else if((_moveX!=0||_moveY!=0)){
                // Joystick : les tirs vont dans la direction de déplacement
                // (on vérifie si une touche de mouvement EST enfoncée maintenant)
                bool moving = IsKeyDown(KEY_W)||IsKeyDown(KEY_S)||IsKeyDown(KEY_A)||IsKeyDown(KEY_D)||
                              IsKeyDown(KEY_UP)||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT);
                if(moving){
                    vx=_moveX; vy=_moveY;
                } else {
                    vx=mx-playerPos->x; vy=my-playerPos->y;
                }
            } else {
                vx=mx-playerPos->x; vy=my-playerPos->y;
            }

            // Normaliser direction tir
            float dl=std::sqrt(vx*vx+vy*vy);
            if(dl>0){vx/=dl;vy/=dl;}
            float tx=playerPos->x+vx*200.f;
            float ty=playerPos->y+vy*200.f;

            // Rotation sprite vers direction de tir
            if(auto*spr=world.getComponent<SpriteComponent>(_player))
                spr->rotation=std::atan2(vy,vx)*180.f/3.14159f;

            // Tir continu (maintenu) - clic ou joystick en mouvement
            bool shoot=IsMouseButtonDown(MOUSE_LEFT_BUTTON)||
                       IsKeyDown(KEY_W)||IsKeyDown(KEY_S)||IsKeyDown(KEY_A)||IsKeyDown(KEY_D)||
                       IsKeyDown(KEY_UP)||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT);
            if(shoot){
                auto*wc=world.getComponent<WeaponComponent>(_player);
                if(wc&&wc->current()) wc->current()->fire(playerPos->x,playerPos->y,tx,ty);
            }
        }

        // Swap
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)||GetMouseWheelMove()!=0){
            auto*wc=world.getComponent<WeaponComponent>(_player);
            if(wc) wc->swap();
        }
        // Reload R
        if(IsKeyPressed(KEY_R)){
            auto*wc=world.getComponent<WeaponComponent>(_player);
            if(wc&&wc->current()) wc->current()->reload();
        }
        // Pickup E
        if(IsKeyPressed(KEY_E)) doPickupWeapon();

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

        // Animation player (bob)
        bool isMoving=IsKeyDown(KEY_W)||IsKeyDown(KEY_S)||IsKeyDown(KEY_A)||IsKeyDown(KEY_D)||
                      IsKeyDown(KEY_UP)||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT);
        if(isMoving){
            _animTimer+=dt;
            if(_animTimer>=1.f/ANIM_FPS){
                _animTimer=0; _animFrame=(_animFrame+1)%ANIM_FRAMES;
                // Mettre à jour le rect source (2 frames dans player_sheet.png)
                if(auto*spr=world.getComponent<SpriteComponent>(_player))
                    spr->left=_animFrame*64.f;
            }
        } else {
            _animFrame=0;
            if(auto*spr=world.getComponent<SpriteComponent>(_player)) spr->left=0.f;
        }

        // Fullscreen F11
        if(IsKeyPressed(KEY_F11)){
            _g->isFullscreen=!_g->isFullscreen;
            if(_g->isFullscreen){
                int mon=GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(mon),GetMonitorHeight(mon));
                ToggleFullscreen();
            } else {
                ToggleFullscreen();
                SetWindowSize(1280,720);
            }
            _render.initRenderTarget(); // Recréer la render texture si besoin
        }

        syncHUD();
    }

    void doPickupWeapon(){
        auto*col=world.getComponent<BoxColliderComponent>(_player); if(!col) return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);
            if(!spr||spr->tag!=EntityTag::PICKUP) continue;
            auto*pwc=world.getComponent<WeaponComponent>(o);
            if(!pwc||!pwc->isPickup) continue;
            auto*plwc=world.getComponent<WeaponComponent>(_player);
            if(plwc){ int slot=1-plwc->activeSlot; plwc->slots[slot]={createWeapon(pwc->pickupWeaponType,&world),pwc->pickupWeaponType}; }
            rm.push_back(o);
        }
        for(Entity e:rm) world.removeEntity(e);
    }

    void collectPickups(){
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        auto*pl =world.getComponent<PlayerComponent>(_player);
        auto*hp =world.getComponent<HealthComponent>(_player);
        auto*wc =world.getComponent<WeaponComponent>(_player);
        if(!col||!pl||!hp) return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o); if(!spr) continue;
            if(spr->tag==EntityTag::XP_ORB){ pl->materials++; rm.push_back(o); }
            else if(spr->tag==EntityTag::PICKUP_AMMO){
                if(wc){ for(auto& s:wc->slots) if(s.weapon) s.weapon->refillAmmo(); }
                rm.push_back(o);
            }
            else if(spr->tag==EntityTag::PICKUP_HP){
                hp->hp=std::min(hp->maxHp,hp->hp+50.f); rm.push_back(o);
            }
            else if(spr->tag==EntityTag::PICKUP_SHIELD){
                pl->hasShield=true; pl->shieldHp=pl->shieldMax; rm.push_back(o);
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
            float dmg=0; bool hit=false;
            if(spr->tag==EntityTag::BULLET_EN){
                auto*b=world.getComponent<BulletComponent>(o);
                if(b) dmg=b->damage; world.removeEntity(o); hit=true;
            } else if(spr->tag==EntityTag::ENEMY||spr->tag==EntityTag::BOSS){
                auto*ai=world.getComponent<EnemyAIComponent>(o);
                dmg=(ai?ai->damage:0.1f)*dt*2.f; hit=true;
            }
            if(hit&&dmg>0){
                if(pl->hasShield&&pl->shieldHp>0){
                    pl->shieldHp-=dmg;
                    if(pl->shieldHp<=0){pl->shieldHp=0;pl->hasShield=false;}
                } else { hp->hp-=dmg; }
                pl->isInvincible=true; pl->invincTimer=0.5f;
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
            spr->tint=(pl->isInvincible&&(int)(pl->invincTimer*12)%2==0)?Color{255,80,80,160}:WHITE;
    }

    void checkDeath(){
        auto*hp=world.getComponent<HealthComponent>(_player);
        if(hp&&hp->hp<=0.f){_sm->changeScene(SceneType::GAMEOVER);return;}
        auto*wv=world.getComponent<WaveComponent>(_waveEnt);
        if(wv&&wv->state==WaveState::VICTORY){_sm->changeScene(SceneType::GAMEOVER);return;}
    }

    void syncHUD(){
        if(auto*hp=world.getComponent<HealthComponent>(_player)){_hp=hp->hp;_maxHp=hp->maxHp;}
        if(auto*pl=world.getComponent<PlayerComponent>(_player)){
            _mats=pl->materials;_kills=pl->kills;
            _hasShield=pl->hasShield;_shieldHp=pl->shieldHp;_shieldMax=pl->shieldMax;
        }
        if(auto*wc=world.getComponent<WeaponComponent>(_player)){
            _ammo=wc->ammo();_maxAmmo=wc->maxAmmo();
            _wname=wc->weaponName();_reloading=wc->reloading();
            int other=1-wc->activeSlot;
            _w2name=wc->slots[other].weapon?wc->slots[other].weapon->getName():"--";
        }
        if(auto*wv=world.getComponent<WaveComponent>(_waveEnt)){
            _wave_n=wv->currentWave;_wstate=wv->state;_betweenT=wv->betweenTimer;
        }
    }

    // ── Render ─────────────────────────────────────────────────
    void render(float dt) override {
        _render.update(dt);
        drawShield();
        drawEnemyHP();
        drawHUD();
    }

    // Bouclier : cercle bleu permanent autour du joueur
    void drawShield(){
        auto*pl=world.getComponent<PlayerComponent>(_player);
        auto*pos=world.getComponent<PositionComponent>(_player);
        if(!pl||!pos||!pl->hasShield||pl->shieldHp<=0.f) return;

        // Convertir coords virtuelles -> screen
        float scX=(float)GetScreenWidth()/1920.f;
        float scY=(float)GetScreenHeight()/1080.f;
        float sc=std::min(scX,scY);
        float ox=(GetScreenWidth() -1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;

        float cx=(float)(ox+pos->x*sc);
        float cy=(float)(oy+pos->y*sc);
        float ratio=_shieldMax>0?_shieldHp/_shieldMax:0;

        // Cercle de fond bleu semi-transparent
        DrawCircle((int)cx,(int)cy,(int)(44*sc),{0,80,200,30});
        // Cercle de bordure bleu brillant + pulsation
        float pulse=1.f+0.08f*std::sin(GetTime()*4.f);
        float r=44.f*sc*pulse;
        // Couleur selon HP restants
        Color shieldCol={
            (unsigned char)(60*(1-ratio)),
            (unsigned char)(120*ratio),
            (unsigned char)(255),
            200
        };
        DrawCircleLines((int)cx,(int)cy,(int)r,shieldCol);
        DrawCircleLines((int)cx,(int)cy,(int)(r+2),{shieldCol.r,shieldCol.g,shieldCol.b,80});
        DrawCircleLines((int)cx,(int)cy,(int)(r+4),{shieldCol.r,shieldCol.g,shieldCol.b,40});
    }

    void drawEnemyHP(){
        float scX=(float)GetScreenWidth()/1920.f;
        float scY=(float)GetScreenHeight()/1080.f;
        float sc=std::min(scX,scY);
        float ox=(GetScreenWidth() -1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;

        for(Entity e:world.getContainer<EnemyAIComponent>().getEntities()){
            auto*pos=world.getComponent<PositionComponent>(e);
            auto*hp =world.getComponent<HealthComponent>(e);
            auto*ai =world.getComponent<EnemyAIComponent>(e);
            auto*spr=world.getComponent<SpriteComponent>(e);
            if(!pos||!hp||!ai) continue;
            float ratio=hp->maxHp>0?hp->hp/hp->maxHp:0;
            float barW=ai->isBoss?100.f:55.f;
            float barH=ai->isBoss?12.f:7.f;
            float sprH=(spr?spr->height*spr->scale:64.f);
            float sx=(float)(ox+(pos->x-barW/2)*sc);
            float sy=(float)(oy+(pos->y-sprH/2-barH-8)*sc);
            float sw2=barW*sc,sh2=barH*sc;
            DrawRectangle((int)sx,(int)sy,(int)sw2,(int)sh2,DARKGRAY);
            Color hcol=ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED;
            if(ai->isBoss) hcol=ratio>0.5f?ORANGE:RED;
            DrawRectangle((int)sx,(int)sy,(int)(sw2*ratio),(int)sh2,hcol);
            DrawRectangleLines((int)sx,(int)sy,(int)sw2,(int)sh2,WHITE);
            if(ai->isBoss){
                std::string bs="BOSS "+std::to_string((int)hp->hp)+"/"+std::to_string((int)hp->maxHp);
                int bsz=(int)(11*sc); if(bsz<8) bsz=8;
                DrawText(bs.c_str(),(int)sx,(int)(sy-bsz-2),bsz,ORANGE);
            }
        }
    }

    void drawHUD(){
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,sh-56,sw,56,{0,0,0,225});
        DrawLine(0,sh-56,sw,sh-56,{40,40,60,255});
        int y=sh-56+10, sz=20;

        // HP
        float ratio=_maxHp>0?_hp/_maxHp:0;
        DrawRectangle(8,y,200,32,DARKGRAY);
        DrawRectangle(8,y,(int)(200*ratio),32,ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED);
        DrawRectangleLines(8,y,200,32,WHITE);
        DrawText(TextFormat("HP %.0f/%.0f",_hp,_maxHp),14,y+6,sz,WHITE);

        // Bouclier HUD
        if(_hasShield){
            float sr=_shieldMax>0?_shieldHp/_shieldMax:0;
            DrawRectangle(8,y-18,200,12,DARKGRAY);
            DrawRectangle(8,y-18,(int)(200*sr),12,{60,120,255,255});
            DrawRectangleLines(8,y-18,200,12,{100,180,255,255});
            DrawText(TextFormat("SHIELD %.0f",_shieldHp),10,y-16,10,{150,200,255,255});
        }

        DrawLine(216,sh-56,216,sh,{40,40,60,255});

        // Arme
        int ax=224;
        DrawText(TextFormat("[%s]",_wname.c_str()),ax,y,sz,_reloading?ORANGE:YELLOW);
        int ww=MeasureText(TextFormat("[%s]",_wname.c_str()),sz);
        DrawText(TextFormat("%d/%d",_ammo,_maxAmmo),ax+ww+6,y,sz,WHITE);
        if(_reloading) DrawText("RECHARGEMENT...",ax,y-18,13,ORANGE);
        DrawText(TextFormat("2e:[%s]",_w2name.c_str()),ax,y+sz+2,13,GRAY);
        DrawLine(ax+200,sh-56,ax+200,sh,{40,40,60,255});

        // Vague
        int vx=ax+208;
        Color wc2=_wstate==WaveState::BETWEEN_WAVES?ORANGE:WHITE;
        DrawText(TextFormat("Vague %d/20",_wave_n),vx,y,sz,wc2);
        if(_wstate==WaveState::BETWEEN_WAVES)
            DrawText(TextFormat("Prochain boss dans %.0fs",_betweenT),vx,y-18,13,ORANGE);
        DrawLine(vx+165,sh-56,vx+165,sh,{40,40,60,255});

        // Stats
        int sx2=vx+173;
        DrawText(TextFormat("Mat:%d",_mats),sx2,y,sz,GREEN);
        DrawText(TextFormat("Kills:%d",_kills),sx2+90,y,sz,ORANGE);

        // Aide
        const char* h="[R]Reload [E]Pickup [Scroll]Swap [Echap]Menu";
        int fsz=13;
        DrawText(h,sw-MeasureText(h,fsz)-8,sh-fsz-6,fsz,{70,70,70,255});

        // Crosshair
        Vector2 m=GetMousePosition();
        int cx=(int)m.x,cy=(int)m.y;
        DrawLine(cx-14,cy,cx-5,cy,{255,255,255,200});
        DrawLine(cx+5, cy,cx+14,cy,{255,255,255,200});
        DrawLine(cx,cy-14,cx,cy-5, {255,255,255,200});
        DrawLine(cx,cy+5, cx,cy+14,{255,255,255,200});
        DrawCircleLines(cx,cy,5,{255,255,255,160});
    }
};
