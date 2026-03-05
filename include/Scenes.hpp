#pragma once
#include "Engine.hpp"
#include "IGame.hpp"
#include <memory>
#include <cmath>
#include <string>

std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

#pragma once
#include "Engine.hpp"
#include "IGame.hpp"
#include <memory>
#include <cmath>
#include <string>

std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

// ================================================================
// SPLASH SCENE — Cinématique de démarrage
// Activision (1s) -> TiMi (1s) -> Loading (barre) -> Menu
// ================================================================
class SplashScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;

    enum class Phase {
        ACTIVISION,   // fade in/out logo Activision
        TIMI,         // fade in/out logo TiMi
        LOADING,      // barre de chargement style pixel
    };

    Phase _phase      = Phase::ACTIVISION;
    float _timer      = 0.f;
    float _fadeAlpha  = 0.f;   // 0=noir, 1=pleine image
    float _loadPct    = 0.f;   // progression barre 0..1
    bool  _doneFade   = false;

    // Timings
    static constexpr float FADE_IN   = 0.4f;
    static constexpr float HOLD      = 0.7f;
    static constexpr float FADE_OUT  = 0.4f;
    static constexpr float LOGO_TOTAL = FADE_IN + HOLD + FADE_OUT; // 1.5s par logo

    // Texture indices cinématique
    static const int TEX_ACTIVISION = 32;
    static const int TEX_TIMI       = 33;
    static const int TEX_LOADBAR    = 35;

    // Loading bar style (copié du screenshot)
    // Fond: RGB(31,29,50)  Vert: RGB(113,210,26)  Vert clair: RGB(120,240,0)
    static constexpr int BAR_BG_R=24,  BAR_BG_G=22,  BAR_BG_B=44;   // fond bleu-nuit
    static constexpr int BAR_FG_R=113, BAR_FG_G=210, BAR_FG_B=26;   // vert principal
    static constexpr int BAR_HI_R=150, BAR_HI_G=255, BAR_HI_B=30;   // reflet haut

public:
    SplashScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Splash"), _sm(&sm), _g(&g) {}

    void onEnter() override {
        _phase=Phase::ACTIVISION; _timer=0; _fadeAlpha=0; _loadPct=0; _doneFade=false;
    }
    void onExit() override {}

    void update(float dt) override {
        _timer += dt;

        // Skip avec espace ou clic
        if (IsKeyPressed(KEY_SPACE)||IsKeyPressed(KEY_ENTER)||IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (_phase != Phase::LOADING) { _phase=Phase::LOADING; _timer=0; _loadPct=0; return; }
        }

        switch (_phase) {
        case Phase::ACTIVISION:
        case Phase::TIMI: {
            // Fade in
            if (_timer < FADE_IN)
                _fadeAlpha = _timer / FADE_IN;
            // Hold
            else if (_timer < FADE_IN + HOLD)
                _fadeAlpha = 1.f;
            // Fade out
            else if (_timer < LOGO_TOTAL)
                _fadeAlpha = 1.f - (_timer - FADE_IN - HOLD) / FADE_OUT;
            else {
                _fadeAlpha = 0.f;
                // Passer au logo suivant
                if (_phase == Phase::ACTIVISION) { _phase=Phase::TIMI; _timer=0; }
                else { _phase=Phase::LOADING; _timer=0; _loadPct=0; }
            }
            break;
        }
        case Phase::LOADING: {
            // Barre de chargement qui progresse sur 2 secondes
            float speed = 1.f / 2.0f;   // 2 secondes pour tout charger
            _loadPct = std::min(1.f, _timer * speed);
            if (_loadPct >= 1.f && _timer > 2.2f) {
                // Fade out puis aller au menu
                _sm->changeScene(SceneType::MENU);
            }
            break;
        }
        }
    }

    void render(float dt) override {
        (void)dt;
        int sw = GetScreenWidth(), sh = GetScreenHeight();

        switch (_phase) {
        case Phase::ACTIVISION:
        case Phase::TIMI: {
            // Fond noir
            DrawRectangle(0,0,sw,sh,BLACK);
            // Image splash centrée
            int texIdx = (_phase==Phase::ACTIVISION) ? TEX_ACTIVISION : TEX_TIMI;
            if ((int)_g->_allSprites.size() > texIdx) {
                Texture2D& tex = _g->_allSprites[texIdx];
                // Afficher en plein écran avec letterbox
                float scX=(float)sw/tex.width, scY=(float)sh/tex.height;
                float sc=std::min(scX,scY);
                float dw=tex.width*sc, dh=tex.height*sc;
                float ox=(sw-dw)/2, oy=(sh-dh)/2;
                unsigned char alpha=(unsigned char)(_fadeAlpha*255);
                DrawTexturePro(tex,
                    {0,0,(float)tex.width,(float)tex.height},
                    {ox,oy,dw,dh},
                    {0,0},0,
                    {255,255,255,alpha});
            }
            break;
        }
        case Phase::LOADING: {
            // Fond noir
            DrawRectangle(0,0,sw,sh,BLACK);

            // Titre
            int tsz = sh/12;
            const char* title = "ZIMEX";
            int tw = MeasureText(title, tsz);
            DrawText(title, sw/2-tw/2+2, (int)(sh*0.25f)+2, tsz, {80,40,0,180});
            DrawText(title, sw/2-tw/2,   (int)(sh*0.25f),   tsz, YELLOW);

            // Sous-titre
            const char* sub = "Chargement en cours...";
            int ssz = sh/30;
            DrawText(sub, sw/2-MeasureText(sub,ssz)/2, (int)(sh*0.60f), ssz, {120,120,120,255});

            // ── BARRE DE CHARGEMENT style pixel ─────────────────
            int barW = (int)(sw * 0.55f);   // largeur totale
            int barH = (int)(sh * 0.05f);   // hauteur
            int barX = sw/2 - barW/2;
            int barY = (int)(sh * 0.68f);
            int borderW = 4;

            // Fond de la barre (bleu nuit)
            DrawRectangle(barX, barY, barW, barH,
                {BAR_BG_R, BAR_BG_G, BAR_BG_B, 255});

            // Partie remplie (vert)
            int filledW = (int)(barW * _loadPct);
            if (filledW > borderW*2) {
                // Corps vert
                DrawRectangle(barX+borderW, barY+borderW,
                              filledW-borderW*2, barH-borderW*2,
                              {BAR_FG_R, BAR_FG_G, BAR_FG_B, 255});
                // Reflet lumineux (ligne du haut)
                int hiH = std::max(2, barH/6);
                DrawRectangle(barX+borderW, barY+borderW,
                              filledW-borderW*2, hiH,
                              {BAR_HI_R, BAR_HI_G, BAR_HI_B, 200});
                // Ligne sombre en bas
                DrawRectangle(barX+borderW, barY+barH-borderW*2,
                              filledW-borderW*2, borderW,
                              {60, 140, 10, 200});
                // Effet pixel : segments verticaux sombres tous les 8px
                for (int sx=barX+borderW; sx<barX+filledW-8; sx+=12) {
                    DrawRectangle(sx, barY+borderW, 2, barH-borderW*2,
                                  {0, 0, 0, 30});
                }
            }

            // Bordure extérieure pixelisée
            DrawRectangleLines(barX, barY, barW, barH, {60,60,80,255});
            // Coin intérieur
            DrawRectangle(barX+borderW, barY+borderW,
                          barW-borderW*2, barH-borderW*2, {0,0,0,0});
            DrawRectangleLines(barX+borderW, barY+borderW,
                               barW-borderW*2, barH-borderW*2, {40,40,60,180});

            // Pourcentage
            std::string pct = std::to_string((int)(_loadPct*100)) + "%";
            int psz = barH*6/10;
            DrawText(pct.c_str(),
                     barX+barW+10, barY+(barH-psz)/2, psz,
                     {BAR_FG_R, BAR_FG_G, BAR_FG_B, 255});

            // Hint
            if (_loadPct >= 1.f) {
                const char* ready = "Appuie sur ESPACE pour continuer";
                int rsz = sh/32;
                float a = 0.5f + 0.5f*std::sin(GetTime()*3.f);
                DrawText(ready, sw/2-MeasureText(ready,rsz)/2, (int)(sh*0.82f),
                         rsz, {255,255,255,(unsigned char)(a*220)});
            }
            break;
        }
        }
    }
};


// ================================================================
// INFO SCENE
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
            float ny=GetMousePosition().y/GetScreenHeight();
            float nx=GetMousePosition().x/GetScreenWidth();
            if(ny>0.88f){
                if(nx<0.35f) _page=(_page-1+PAGES)%PAGES;
                else if(nx>0.65f) _page=(_page+1)%PAGES;
                else _sm->changeScene(SceneType::MENU);
            }
        }
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int g=0;g<sw;g+=sw/24) DrawLine(g,0,g,sh,{10,10,28,255});
        for(int g=0;g<sh;g+=sh/18) DrawLine(0,g,sw,g,{10,10,28,255});
        int tsz=sh/14;
        const char* t="COMMENT JOUER";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+2,sh/16+2,tsz,{0,0,0,140});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/16,  tsz,YELLOW);
        int lsz=sh/26, lx=sw/16, ly=(int)(sh*0.18f), ls=(int)(lsz*1.6f);
        Color kc={255,220,50,255}, vc={200,200,200,255}, sc2={100,200,255,255};
        if(_page==0){
            DrawText("── BUT DU JEU ──",lx,ly,lsz,sc2); ly+=ls;
            DrawText("Survivre 10 vagues de monstres !",lx,ly,lsz-2,vc); ly+=ls;
            DrawText("Chaque vague se termine par un BOSS unique. Ramasse des pickups.",lx,ly,lsz-2,vc); ly+=ls*2;
            DrawText("── MOUVEMENT ──",lx,ly,lsz,sc2); ly+=ls;
            struct KV{const char*k;const char*v;};
            KV ctrl[]={
                {"W / Flèche Haut","Aller vers le haut"},
                {"S / Flèche Bas", "Aller vers le bas"},
                {"A / Flèche Gauche","Aller à gauche (sprite face gauche)"},
                {"D / Flèche Droite","Aller à droite (sprite face droite)"},
            };
            for(auto& kv:ctrl){DrawText(kv.k,lx,ly,lsz-2,kc);DrawText(kv.v,lx+MeasureText(kv.k,lsz-2)+12,ly,lsz-2,vc);ly+=ls;}
            ly+=ls/2;
            DrawText("── TIR & ARMES ──",lx,ly,lsz,sc2); ly+=ls;
            KV fire[]={
                {"Souris (position)","Visée - le joueur regarde vers le curseur"},
                {"Clic Gauche","Tirer vers la souris"},
                {"Clic Droit","Changer d'arme (slot 2)"},
                {"Molette","Changer d'arme"},
                {"R","Recharger"},
                {"E","Ramasser une arme au sol"},
            };
            for(auto& kv:fire){DrawText(kv.k,lx,ly,lsz-2,kc);DrawText(kv.v,lx+MeasureText(kv.k,lsz-2)+12,ly,lsz-2,vc);ly+=ls;}
            ly+=ls/2;
            DrawText("── DIVERS ──",lx,ly,lsz,sc2); ly+=ls;
            KV misc[]={
                {"F11","Plein écran"},
                {"+/-","Volume (aussi dans Paramètres)"},
                {"Echap","Retour au menu"},
            };
            for(auto& kv:misc){DrawText(kv.k,lx,ly,lsz-2,kc);DrawText(kv.v,lx+MeasureText(kv.k,lsz-2)+12,ly,lsz-2,vc);ly+=ls;}
        } else {
            int c1=sw/16, c2=sw/2+sw/16, cy=ly;
            DrawText("── ARMES ──",c1,cy,lsz,sc2); cy+=ls;
            struct W{const char*n;Color c;const char*d;};
            W ww[]={{"Pistol",YELLOW,"20 dmg | 120 balles | semi-auto"},{"Shotgun",ORANGE,"8dmg×7 | 60 balles | cône 40°"},{"Sniper",SKYBLUE,"60 dmg | 50 balles | perce x3"},{"AK-47",GREEN,"12 dmg | 300 balles | rafale"},{"Bomb",RED,"80+50 AoE | 30 balles | explosion"}};
            for(auto& w:ww){DrawText(w.n,c1,cy,lsz-2,w.c);DrawText(w.d,c1+MeasureText(w.n,lsz-2)+10,cy,lsz-2,vc);cy+=ls;}
            cy+=ls/2;
            DrawText("── PICKUPS ──",c1,cy,lsz,sc2); cy+=ls;
            struct P{const char*k;Color c;const char*v;};
            P pp[]={{"Orbe vert",GREEN,"Matériaux +1"},{"Jaune",YELLOW,"Recharge munitions"},{"Vert",{50,255,100,255},"Soin +50 HP"},{"Bleu",{80,150,255,255},"Bouclier 360 HP"}};
            for(auto& p:pp){DrawText(p.k,c1,cy,lsz-2,p.c);DrawText(p.v,c1+MeasureText(p.k,lsz-2)+10,cy,lsz-2,vc);cy+=ls;}
            cy=ly;
            DrawText("── ENNEMIS & BOSS ──",c2,cy,lsz,{255,100,100,255}); cy+=ls;
            DrawText("Sprites Brotato réels (30 types)",c2,cy,lsz-2,vc); cy+=ls;
            DrawText("Vague N = N×10 ennemis",c2,cy,lsz-2,vc); cy+=ls;
            DrawText("Boss unique à la FIN de chaque vague",c2,cy,lsz-2,vc); cy+=ls;
            DrawText("HP boss : 400+vague×80",c2,cy,lsz-2,vc); cy+=ls;
            cy+=ls/2;
            DrawText("── BOUCLIER ──",c2,cy,lsz,{80,150,255,255}); cy+=ls;
            DrawText("Cercle bleu pulsant autour du joueur",c2,cy,lsz-2,vc); cy+=ls;
            DrawText("360 HP, absorbe tous les dégâts",c2,cy,lsz-2,vc); cy+=ls;
        }
        int nsz=sh/24;
        DrawLine(0,sh-50,sw,sh-50,{40,40,60,255});
        if(_page>0)       DrawText("< Précédent",sw/16,sh-40,nsz,{100,200,255,255});
        if(_page<PAGES-1) DrawText("Suivant >",sw-MeasureText("Suivant >",nsz)-sw/16,sh-40,nsz,{100,200,255,255});
        const char* r="[ Retour Menu ]";
        DrawText(r,sw/2-MeasureText(r,nsz)/2,sh-40,nsz,GRAY);
        std::string pg=std::to_string(_page+1)+"/"+std::to_string(PAGES);
        DrawText(pg.c_str(),sw-MeasureText(pg.c_str(),nsz)-10,10,nsz,DARKGRAY);
    }
};

// ================================================================
// MENU SCENE
// ================================================================
// ================================================================
// MENU SCENE — Fond Brotato + boutons
// ================================================================
class MenuScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _anim=0;
    static const int TEX_MENU_BG = 34;
public:
    MenuScene(SceneManager& sm,GlobalComponent& g):Scene("Menu"),_sm(&sm),_g(&g){}
    void onEnter() override {_anim=0;}
    void onExit()  override {}
    void update(float dt) override {
        _anim+=dt;
        if(IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD))
            SetMasterVolume(_g->masterVolume=std::min(1.f,_g->masterVolume+0.05f));
        if(IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT))
            SetMasterVolume(_g->masterVolume=std::max(0.f,_g->masterVolume-0.05f));
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

        // ── Fond Brotato ─────────────────────────────────────
        if((int)_g->_allSprites.size()>TEX_MENU_BG){
            Texture2D& bg=_g->_allSprites[TEX_MENU_BG];
            // Plein écran avec ratio conservé
            float scX=(float)sw/bg.width, scY=(float)sh/bg.height;
            float sc=std::max(scX,scY);   // fill (pas letterbox)
            float dw=bg.width*sc, dh=bg.height*sc;
            float ox=(sw-dw)/2, oy=(sh-dh)/2;
            DrawTexturePro(bg,{0,0,(float)bg.width,(float)bg.height},
                           {ox,oy,dw,dh},{0,0},0,WHITE);
            // Overlay sombre pour lisibilité
            DrawRectangle(0,0,sw,sh,{0,0,0,140});
        } else {
            DrawRectangle(0,0,sw,sh,{5,5,15,255});
            for(int gx=0;gx<sw;gx+=sw/24) DrawLine(gx,0,gx,sh,{10,10,28,255});
            for(int gy=0;gy<sh;gy+=sh/18) DrawLine(0,gy,sw,gy,{10,10,28,255});
        }

        // ── Titre ZIMEX ──────────────────────────────────────
        float pulse=1.f+0.04f*std::sin(_anim*3.f);
        int tsz=(int)(sh/7*pulse);
        const char* title="ZIMEX";
        // Ombre portée
        DrawText(title,sw/2-MeasureText(title,tsz)/2+4,sh/10+4,tsz,{0,0,0,200});
        DrawText(title,sw/2-MeasureText(title,tsz)/2+2,sh/10+2,tsz,{80,40,0,180});
        DrawText(title,sw/2-MeasureText(title,tsz)/2,  sh/10,  tsz,YELLOW);

        // ── Boutons ───────────────────────────────────────────
        struct Btn{float ny1,ny2;const char*lbl;Color bg;Color fg;};
        Btn btns[]={
            {0.36f,0.43f,"> Jouer",        {20,80,20,230},  GREEN},
            {0.46f,0.53f,"> Infos / Aide", {15,45,90,230},  {100,200,255,255}},
            {0.56f,0.63f,"> Paramètres",   {15,15,70,230},  WHITE},
            {0.66f,0.73f,"> Quitter",      {80,15,15,230},  RED},
        };
        for(auto& b:btns){
            int bx=(int)(sw*0.33f),by=(int)(sh*b.ny1);
            int bw=(int)(sw*0.34f),bh=(int)(sh*(b.ny2-b.ny1));
            // Ombre
            DrawRectangle(bx+3,by+3,bw,bh,{0,0,0,120});
            DrawRectangle(bx,by,bw,bh,b.bg);
            DrawRectangleLines(bx,by,bw,bh,{120,120,140,220});
            int lsz=bh*6/10;
            DrawText(b.lbl,bx+14,by+bh/2-lsz/2,lsz,b.fg);
        }

        // Volume
        int vsz=sh/30;
        DrawText(TextFormat("Vol:%d%%  (+/-)",(int)(_g->masterVolume*100)),
                 (int)(sw*0.02f),sh-vsz*2,vsz,{140,140,140,255});

        const char* hint="Clic G: Tirer | Scroll: Changer arme | R: Recharger | E: Pickup arme";
        int fsz=sh/38;
        DrawText(hint,sw/2-MeasureText(hint,fsz)/2,sh-fsz*2,fsz,{100,100,100,255});
    }
};

class SettingsScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _brightness=1.f;
    bool  _draggingVol=false;
public:
    SettingsScene(SceneManager& sm,GlobalComponent& g):Scene("Settings"),_sm(&sm),_g(&g){}
    void onEnter() override {}
    void onExit()  override {}
    void update(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        // Volume : touches +/-
        if(IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD))
            SetMasterVolume(_g->masterVolume=std::min(1.f,_g->masterVolume+0.05f));
        if(IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT))
            SetMasterVolume(_g->masterVolume=std::max(0.f,_g->masterVolume-0.05f));
        // Volume : slider souris
        int bx=(int)(sw*0.44f),by=(int)(sh*0.30f),bw=(int)(sw*0.42f),bh=sh/20;
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 m=GetMousePosition();
            if(m.x>=bx&&m.x<=bx+bw&&m.y>=by&&m.y<=by+bh) _draggingVol=true;
        }
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) _draggingVol=false;
        if(_draggingVol){
            float nx=(GetMousePosition().x-bx)/(float)bw;
            _g->masterVolume=std::max(0.f,std::min(1.f,nx));
            SetMasterVolume(_g->masterVolume);
        }
        // Luminosité
        if(IsKeyPressed(KEY_UP))   _brightness=std::min(1.f,_brightness+0.05f);
        if(IsKeyPressed(KEY_DOWN)) _brightness=std::max(0.1f,_brightness-0.05f);
        // Fullscreen F11
        if(IsKeyPressed(KEY_F11)){
            _g->isFullscreen=!_g->isFullscreen;
            if(_g->isFullscreen){ int mon=GetCurrentMonitor(); SetWindowSize(GetMonitorWidth(mon),GetMonitorHeight(mon)); ToggleFullscreen(); }
            else { ToggleFullscreen(); SetWindowSize(1280,720); }
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            float ny=GetMousePosition().y/(float)sh;
            if(ny>=0.83f&&ny<=0.90f) _sm->changeScene(SceneType::MENU);
        }
        if(IsKeyPressed(KEY_ESCAPE)) _sm->changeScene(SceneType::MENU);
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        if(_brightness<1.f){ unsigned char a=(unsigned char)((1.f-_brightness)*200.f); DrawRectangle(0,0,sw,sh,{0,0,0,a}); }
        int tsz=sh/9; const char* t="PARAMÈTRES";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+2,sh/12+2,tsz,{0,0,0,120});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/12,  tsz,WHITE);
        int lsz=sh/20, lx=(int)(sw*0.14f), ly=(int)(sh*0.28f), ls=lsz+lsz/2;
        int bx=(int)(sw*0.44f), bw=(int)(sw*0.42f), bh=lsz;
        // Volume avec slider cliquable
        DrawText("Volume",lx,ly,lsz,WHITE);
        DrawText("(clic+glisse ou +/-)",lx+MeasureText("Volume",lsz)+10,ly,lsz-6,DARKGRAY);
        DrawRectangle(bx,ly,bw,bh,DARKGRAY);
        DrawRectangle(bx,ly,(int)(bw*_g->masterVolume),bh,GREEN);
        // Poignée slider
        int hx=bx+(int)(bw*_g->masterVolume);
        DrawRectangle(hx-4,ly-4,8,bh+8,WHITE);
        DrawRectangleLines(bx,ly,bw,bh,{100,100,120,255});
        DrawText(TextFormat("%d%%",(int)(_g->masterVolume*100)),bx+bw+10,ly,lsz,GREEN);
        ly+=ls;
        DrawText("Luminosité (↑/↓)",lx,ly,lsz,WHITE);
        DrawRectangle(bx,ly,bw,bh,DARKGRAY);
        DrawRectangle(bx,ly,(int)(bw*_brightness),bh,YELLOW);
        DrawRectangleLines(bx,ly,bw,bh,{100,100,120,255});
        DrawText(TextFormat("%d%%",(int)(_brightness*100)),bx+bw+10,ly,lsz,YELLOW);
        ly+=ls;
        bool fs=_g->isFullscreen;
        DrawText(TextFormat("Plein écran [F11] : %s",fs?"ON":"OFF"),lx,ly,lsz,fs?GREEN:GRAY);
        int bky=(int)(sh*0.84f),bkh=(int)(sh*0.07f),bkw=(int)(sw*0.34f),bkx=sw/2-bkw/2;
        DrawRectangle(bkx,bky,bkw,bkh,{20,20,80,220});
        DrawRectangleLines(bkx,bky,bkw,bkh,{60,60,120,255});
        const char*bk="< Retour"; int bksz=bkh*6/10;
        DrawText(bk,bkx+bkw/2-MeasureText(bk,bksz)/2,bky+bkh/2-bksz/2,bksz,{100,200,255,255});
    }
};

// ================================================================
// GAME OVER
// ================================================================
class GameOverScene : public Scene {
    SceneManager* _sm; bool _victory; float _timer=0;
public:
    GameOverScene(SceneManager& sm,bool v=false):Scene(v?"Victory":"GameOver"),_sm(&sm),_victory(v){}
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
        for(int y=0;y<sh;y+=4) DrawRectangle(0,y,sw,2,{0,0,0,40});
        int tsz=sh/7; const char* t=_victory?"VICTOIRE !":"GAME OVER";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+4,sh/3+4,tsz,{0,0,0,200});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/3,  tsz,_victory?YELLOW:RED);
        if(_victory){ const char*s="10 Vagues Complétées !"; int ssz=sh/16; DrawText(s,sw/2-MeasureText(s,ssz)/2,(int)(sh*0.55f),ssz,GREEN); }
        if(_timer>0.8f){
            float a=std::min(1.f,(_timer-0.8f)/0.5f);
            const char*sub="Appuie sur ENTRÉE ou clique pour revenir au menu"; int ssz=sh/26;
            DrawText(sub,sw/2-MeasureText(sub,ssz)/2,(int)(sh*0.72f),ssz,{255,255,255,(unsigned char)(a*255)});
        }
    }
};

// ================================================================
// GAME SCENE
// ================================================================
// ================================================================
// GAME SCENE — Player animé 4 directions, visée souris, weapon overlay
// ================================================================
// ================================================================
// GAME SCENE — Player animé 4 directions + arme fusionnée
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

    // ── Animation player ──────────────────────────────────────
    // Méga sheets player_<weapon>.png : 288x256
    //   Row 0 (top=  0) = DOWN   6×48px
    //   Row 1 (top= 64) = UP     6×48px
    //   Row 2 (top=128) = RIGHT  6×48px
    //   Row 3 (top=192) = LEFT   6×48px
    static const int FRAME_W=48, FRAME_H=64, N_FRAMES=6;

    // Indices textures des player sheets
    // [0]=bare [1]=pistol [2]=shotgun [3]=sniper [4]=ak47 [5]=bomb
    static int weaponTexIndex(const std::string& type) {
        if(type=="pistol")  return 1;
        if(type=="shotgun") return 2;
        if(type=="sniper")  return 3;
        if(type=="ak47")    return 4;
        if(type=="bomb")    return 5;
        return 1; // défaut pistol
    }
    // Indices textures ennemis & boss
    static const int ENEMY_TEX_BASE=17, ENEMY_TEX_COUNT=8;
    static const int BOSS_TEX_BASE=25,  BOSS_TEX_COUNT=7;
    // Indices pickups
    static const int TEX_PISTOL=10, TEX_SHOTGUN=11, TEX_SNIPER=12, TEX_ROCKET=13;
    static const int TEX_XP_ORB=14;

    enum class Dir { DOWN=0, UP=1, RIGHT=2, LEFT=3 };
    Dir   _dir=Dir::DOWN;
    int   _frame=0;
    float _animT=0.f, _animFPS=9.f;
    bool  _moving=false;

    std::string _carriedType="pistol";
    float       _aimAngle=0.f;

    // HUD
    float _hp=200,_maxHp=200,_shieldHp=0,_shieldMax=360;
    bool  _hasShield=false;
    int   _ammo=0,_maxAmmo=0,_kills=0,_mats=0,_wave_n=1;
    bool  _reloading=false;
    std::string _wname="Pistol",_w2name="Shotgun";
    WaveState _wstate=WaveState::FIGHTING;
    float _betweenT=0;

public:
    GameScene(SceneManager& sm,GlobalComponent& g):Scene("Game"),_sm(&sm),_g(&g){}

    void initSystems(){
        auto W=[&](System& s){s.setWorld(world);};
        W(_render); _render.setGlobal(*_g); _render.initRenderTarget();
        W(_input); W(_collision); W(_physics); W(_health);
        W(_animation); _animation.setGlobal(*_g);
        W(_weapon);    _weapon.setGlobal(*_g);
        W(_enemy);     _enemy.setGlobal(*_g); _enemy.setSceneManager(*_sm);
        W(_bullet);
        W(_wave);      _wave.setGlobal(*_g);  _wave.setSceneManager(*_sm);
        W(_sound);     _sound.setGlobal(*_g);

        _ctx.bind(KEY_W,    [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->y-=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_S,    [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->y+=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_A,    [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->x-=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_D,    [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->x+=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_UP,   [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->y-=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_DOWN, [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->y+=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_LEFT, [](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->x-=pl->speed*GetFrameTime();});
        _ctx.bind(KEY_RIGHT,[](World& w,Entity e){if(auto*p=w.getComponent<PositionComponent>(e))if(auto*pl=w.getComponent<PlayerComponent>(e))p->x+=pl->speed*GetFrameTime();});
        _input.setInputContext(&_ctx);
    }

    void spawnBackground(){
        Entity bg=world.createEntity(); PositionComponent p; p.x=0;p.y=0; world.addComponent(bg,p);
        RectangleComponent r; r.width=1920;r.height=1080;r.inlineColor={5,5,15,255};r.tickness=0;r.layer=0; world.addComponent(bg,r);
        for(int gx=0;gx<1920;gx+=64){Entity e=world.createEntity();PositionComponent gp;gp.x=(float)gx;gp.y=0;world.addComponent(e,gp);RectangleComponent gr;gr.width=1;gr.height=1080;gr.inlineColor={13,13,30,255};gr.tickness=0;gr.layer=0;world.addComponent(e,gr);}
        for(int gy=0;gy<1080;gy+=64){Entity e=world.createEntity();PositionComponent gp;gp.x=0;gp.y=(float)gy;world.addComponent(e,gp);RectangleComponent gr;gr.width=1920;gr.height=1;gr.inlineColor={13,13,30,255};gr.tickness=0;gr.layer=0;world.addComponent(e,gr);}
    }

    void spawnPlayer(){
        _player=world.createEntity();
        PositionComponent pos; pos.x=960;pos.y=540; world.addComponent(_player,pos);
        HealthComponent hp; hp.hp=hp.maxHp=200.f; world.addComponent(_player,hp);
        SpriteComponent spr;
        spr.texturePath=weaponTexIndex("pistol"); // commence avec pistol sheet
        spr.left=0.f; spr.top=0.f;
        spr.width=(float)FRAME_W; spr.height=(float)FRAME_H;
        spr.scale=2.5f;
        spr.offsetX=-(float)FRAME_W*spr.scale/2.f;
        spr.offsetY=-(float)FRAME_H*spr.scale/2.f;
        spr.tag=EntityTag::PLAYER; spr.layer=2; spr.rotation=0.f;
        world.addComponent(_player,spr);
        BoxColliderComponent col; col.isCircle=true; col.radius=20.f;
        col.TagsCollided={(int)EntityTag::BULLET_EN,(int)EntityTag::PICKUP,(int)EntityTag::XP_ORB,(int)EntityTag::ENEMY,(int)EntityTag::BOSS,(int)EntityTag::PICKUP_AMMO,(int)EntityTag::PICKUP_HP,(int)EntityTag::PICKUP_SHIELD};
        world.addComponent(_player,col);
        world.addComponent(_player,InputComponent{});
        PlayerComponent pl; pl.speed=200.f; world.addComponent(_player,pl);
        WeaponComponent wc;
        wc.slots[0]={createWeapon("pistol",&world),"pistol"};
        wc.slots[1]={createWeapon("shotgun",&world),"shotgun"};
        wc.activeSlot=0;
        world.addComponent(_player,wc);
        _dir=Dir::DOWN; _frame=0; _animT=0; _moving=false;
        _carriedType="pistol"; _aimAngle=0.f;
    }

    void spawnWave(){
        _waveEnt=world.createEntity(); PositionComponent p; world.addComponent(_waveEnt,p);
        WaveComponent wc; wc.currentWave=1; wc.maxWaves=10; wc.enemiesLeft=10;
        wc.state=WaveState::FIGHTING; wc.spawnRate=1.2f; wc.betweenMax=4.f;
        world.addComponent(_waveEnt,wc);
    }

    void spawnPickupWeapon(float x,float y,const std::string& type,int tex){
        Entity e=world.createEntity(); PositionComponent p; p.x=x;p.y=y; world.addComponent(e,p);
        float tw=24.f,th=24.f;
        if((int)_g->_allSprites.size()>tex){tw=(float)_g->_allSprites[tex].width;th=(float)_g->_allSprites[tex].height;}
        SpriteComponent s; s.texturePath=tex; s.width=tw;s.height=th; s.scale=2.f;
        s.offsetX=-tw; s.offsetY=-th; s.tag=EntityTag::PICKUP; s.layer=1; world.addComponent(e,s);
        BoxColliderComponent c; c.isCircle=true; c.radius=24.f;
        c.TagsCollided={(int)EntityTag::PLAYER}; world.addComponent(e,c);
        WeaponComponent wc; wc.isPickup=true; wc.pickupWeaponType=type;
        world.addComponent(e,wc);
    }

    void onEnter() override {
        initSystems(); spawnBackground(); spawnPlayer(); spawnWave();
        spawnPickupWeapon(320,240,"sniper",TEX_SNIPER);
        spawnPickupWeapon(1600,840,"bomb",TEX_ROCKET);
        spawnPickupWeapon(1600,240,"ak47",TEX_SHOTGUN);
    }
    void onExit() override { world.clearWorld(); _ctx.clear(); }

    // ── Animation ─────────────────────────────────────────────
    void tickAnimation(float dt){
        bool w=IsKeyDown(KEY_W)||IsKeyDown(KEY_UP);
        bool s=IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN);
        bool a=IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT);
        bool d=IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT);
        _moving=w||s||a||d;
        Dir newDir=_dir;
        if(_moving){
            if(d&&!a)      newDir=Dir::RIGHT;
            else if(a&&!d) newDir=Dir::LEFT;
            else if(w&&!s) newDir=Dir::UP;
            else if(s&&!w) newDir=Dir::DOWN;
            else if(d)     newDir=Dir::RIGHT;
            else if(a)     newDir=Dir::LEFT;
        } else {
            auto*pos=world.getComponent<PositionComponent>(_player);
            if(pos){
                float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
                float ox2=(GetScreenWidth()-1920.f*sc)/2.f, oy2=(GetScreenHeight()-1080.f*sc)/2.f;
                float px=ox2+pos->x*sc, py=oy2+pos->y*sc;
                float dx=GetMousePosition().x-px, dy=GetMousePosition().y-py;
                if(std::fabs(dx)>std::fabs(dy)) newDir=(dx>=0?Dir::RIGHT:Dir::LEFT);
                else newDir=(dy>=0?Dir::DOWN:Dir::UP);
            }
        }
        if(newDir!=_dir){_dir=newDir;_frame=0;_animT=0.f;}
        if(_moving){_animT+=dt;if(_animT>=1.f/_animFPS){_animT=0.f;_frame=(_frame+1)%N_FRAMES;}}
        else{_frame=0;_animT=0.f;}

        // Mettre à jour le sprite : frame + texturePath selon arme
        if(auto*spr=world.getComponent<SpriteComponent>(_player)){
            spr->left=(float)_frame*FRAME_W;
            spr->top=(float)_dir*FRAME_H;
            // Changer la texture selon l'arme portée
            int newTex=weaponTexIndex(_carriedType);
            if(spr->texturePath!=newTex) spr->texturePath=newTex;
        }
    }

    void update(float dt) override {
        if(IsKeyPressed(KEY_ESCAPE)){_sm->changeScene(SceneType::MENU);return;}
        if(IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD)) SetMasterVolume(_g->masterVolume=std::min(1.f,_g->masterVolume+0.05f));
        if(IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT)) SetMasterVolume(_g->masterVolume=std::max(0.f,_g->masterVolume-0.05f));
        if(IsKeyPressed(KEY_F11)){_g->isFullscreen=!_g->isFullscreen;if(_g->isFullscreen){int m=GetCurrentMonitor();SetWindowSize(GetMonitorWidth(m),GetMonitorHeight(m));ToggleFullscreen();}else{ToggleFullscreen();SetWindowSize(1280,720);}_render.initRenderTarget();}

        _input.update(dt);
        if(auto*p=world.getComponent<PositionComponent>(_player)){p->x=std::max(30.f,std::min(p->x,1890.f));p->y=std::max(30.f,std::min(p->y,1050.f));}

        // Visée et tir
        if(auto*pos=world.getComponent<PositionComponent>(_player)){
            float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
            float ox2=(GetScreenWidth()-1920.f*sc)/2.f, oy2=(GetScreenHeight()-1080.f*sc)/2.f;
            float px=ox2+pos->x*sc, py=oy2+pos->y*sc;
            _aimAngle=std::atan2(GetMousePosition().y-py, GetMousePosition().x-px);
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                float tx=pos->x+std::cos(_aimAngle)*200.f;
                float ty=pos->y+std::sin(_aimAngle)*200.f;
                auto*wc=world.getComponent<WeaponComponent>(_player);
                if(wc&&wc->current()) wc->current()->fire(pos->x,pos->y,tx,ty);
            }
        }

        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)||GetMouseWheelMove()!=0){
            auto*wc=world.getComponent<WeaponComponent>(_player);
            if(wc){wc->swap();_carriedType=wc->slots[wc->activeSlot].type;}
        }
        if(IsKeyPressed(KEY_R)){auto*wc=world.getComponent<WeaponComponent>(_player);if(wc&&wc->current())wc->current()->reload();}
        if(IsKeyPressed(KEY_E)) doPickupWeapon();

        _collision.update(dt); _bullet.update(dt); _enemy.update(dt);
        _wave.update(dt); _weapon.update(dt); _sound.update(dt); _animation.update(dt);
        handlePlayerDamage(dt); handleInvinc(dt); collectPickups();
        tickAnimation(dt); checkDeath(); syncHUD();
    }

    void doPickupWeapon(){
        auto*col=world.getComponent<BoxColliderComponent>(_player);if(!col)return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);
            if(!spr||spr->tag!=EntityTag::PICKUP)continue;
            auto*pwc=world.getComponent<WeaponComponent>(o);
            if(!pwc||!pwc->isPickup)continue;
            auto*plwc=world.getComponent<WeaponComponent>(_player);
            if(plwc){int slot=1-plwc->activeSlot;plwc->slots[slot]={createWeapon(pwc->pickupWeaponType,&world),pwc->pickupWeaponType};_carriedType=plwc->slots[plwc->activeSlot].type;}
            rm.push_back(o);
        }
        for(Entity e:rm) world.removeEntity(e);
    }

    void collectPickups(){
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        auto*pl=world.getComponent<PlayerComponent>(_player);
        auto*hp=world.getComponent<HealthComponent>(_player);
        auto*wc=world.getComponent<WeaponComponent>(_player);
        if(!col||!pl||!hp)return;
        std::vector<Entity> rm;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);if(!spr)continue;
            if(spr->tag==EntityTag::XP_ORB){pl->materials++;rm.push_back(o);}
            else if(spr->tag==EntityTag::PICKUP_AMMO){if(wc)for(auto&sl:wc->slots)if(sl.weapon)sl.weapon->refillAmmo();rm.push_back(o);}
            else if(spr->tag==EntityTag::PICKUP_HP){hp->hp=std::min(hp->maxHp,hp->hp+50.f);rm.push_back(o);}
            else if(spr->tag==EntityTag::PICKUP_SHIELD){pl->hasShield=true;pl->shieldHp=pl->shieldMax;rm.push_back(o);}
        }
        for(Entity e:rm) world.removeEntity(e);
    }

    void handlePlayerDamage(float dt){
        auto*pl=world.getComponent<PlayerComponent>(_player);
        auto*col=world.getComponent<BoxColliderComponent>(_player);
        auto*hp=world.getComponent<HealthComponent>(_player);
        if(!pl||!col||!hp||pl->isInvincible)return;
        for(int id:col->EntityCollided){
            Entity o=static_cast<Entity>(id);
            auto*spr=world.getComponent<SpriteComponent>(o);if(!spr)continue;
            float dmg=0;bool hit=false;
            if(spr->tag==EntityTag::BULLET_EN){auto*b=world.getComponent<BulletComponent>(o);if(b)dmg=b->damage;world.removeEntity(o);hit=true;}
            else if(spr->tag==EntityTag::ENEMY||spr->tag==EntityTag::BOSS){auto*ai=world.getComponent<EnemyAIComponent>(o);dmg=(ai?ai->damage:0.1f)*dt*2.f;hit=true;}
            if(hit&&dmg>0){
                if(pl->hasShield&&pl->shieldHp>0){pl->shieldHp-=dmg;if(pl->shieldHp<=0){pl->shieldHp=0;pl->hasShield=false;}}
                else hp->hp-=dmg;
                pl->isInvincible=true;pl->invincTimer=0.5f;
                if(spr->tag==EntityTag::BULLET_EN)break;
            }
        }
    }

    void handleInvinc(float dt){
        auto*pl=world.getComponent<PlayerComponent>(_player);if(!pl)return;
        if(!pl->isInvincible)return;
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
        if(auto*pl=world.getComponent<PlayerComponent>(_player)){_mats=pl->materials;_kills=pl->kills;_hasShield=pl->hasShield;_shieldHp=pl->shieldHp;_shieldMax=pl->shieldMax;}
        if(auto*wc=world.getComponent<WeaponComponent>(_player)){
            _ammo=wc->ammo();_maxAmmo=wc->maxAmmo();_wname=wc->weaponName();_reloading=wc->reloading();
            int ot=1-wc->activeSlot;_w2name=wc->slots[ot].weapon?wc->slots[ot].weapon->getName():"--";
            _carriedType=wc->slots[wc->activeSlot].type;
        }
        if(auto*wv=world.getComponent<WaveComponent>(_waveEnt)){_wave_n=wv->currentWave;_wstate=wv->state;_betweenT=wv->betweenTimer;}
    }

    void render(float dt) override {
        _render.update(dt);
        drawShield();
        drawEnemyHP();
        drawHUD();
    }

    void drawShield(){
        auto*pl=world.getComponent<PlayerComponent>(_player);
        auto*pos=world.getComponent<PositionComponent>(_player);
        if(!pl||!pos||!pl->hasShield||pl->shieldHp<=0.f)return;
        float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
        float ox2=(GetScreenWidth()-1920.f*sc)/2.f, oy2=(GetScreenHeight()-1080.f*sc)/2.f;
        float cx=ox2+pos->x*sc, cy=oy2+pos->y*sc;
        float ratio=_shieldMax>0?_shieldHp/_shieldMax:0;
        float pulse=1.f+0.09f*std::sin(GetTime()*4.f);
        float r=52.f*sc*pulse;
        DrawCircle((int)cx,(int)cy,(int)r,{0,60,200,22});
        Color sc2={(unsigned char)(60*(1-ratio)),(unsigned char)(100+80*ratio),255,210};
        DrawCircleLines((int)cx,(int)cy,(int)r,sc2);
        DrawCircleLines((int)cx,(int)cy,(int)(r+2),{sc2.r,sc2.g,sc2.b,90});
        DrawCircleLines((int)cx,(int)cy,(int)(r+4),{sc2.r,sc2.g,sc2.b,35});
    }

    void drawEnemyHP(){
        float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
        float ox2=(GetScreenWidth()-1920.f*sc)/2.f, oy2=(GetScreenHeight()-1080.f*sc)/2.f;
        for(Entity e:world.getContainer<EnemyAIComponent>().getEntities()){
            auto*pos=world.getComponent<PositionComponent>(e);
            auto*hp=world.getComponent<HealthComponent>(e);
            auto*ai=world.getComponent<EnemyAIComponent>(e);
            auto*spr=world.getComponent<SpriteComponent>(e);
            if(!pos||!hp||!ai)continue;
            float ratio=hp->maxHp>0?hp->hp/hp->maxHp:0;
            float bW=ai->isBoss?100.f:55.f, bH=ai->isBoss?12.f:7.f;
            float spH=spr?spr->height*spr->scale:64.f;
            float sx=ox2+(pos->x-bW/2)*sc, sy=oy2+(pos->y-spH/2-bH-8)*sc;
            float sw=bW*sc, sh=bH*sc;
            DrawRectangle((int)sx,(int)sy,(int)sw,(int)sh,DARKGRAY);
            Color hc=ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED;
            if(ai->isBoss) hc=ratio>0.5f?ORANGE:RED;
            DrawRectangle((int)sx,(int)sy,(int)(sw*ratio),(int)sh,hc);
            DrawRectangleLines((int)sx,(int)sy,(int)sw,(int)sh,WHITE);
            if(ai->isBoss){std::string bs="BOSS "+std::to_string((int)hp->hp)+"/"+std::to_string((int)hp->maxHp);int bsz=(int)(11*sc);if(bsz<8)bsz=8;DrawText(bs.c_str(),(int)sx,(int)(sy-bsz-2),bsz,ORANGE);}
        }
    }

    void drawHUD(){
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,sh-58,sw,58,{0,0,0,235});
        DrawLine(0,sh-58,sw,sh-58,{40,40,65,255});
        int y=sh-58+10, sz=20;
        float ratio=_maxHp>0?_hp/_maxHp:0;
        DrawRectangle(8,y,200,32,DARKGRAY);
        DrawRectangle(8,y,(int)(200*ratio),32,ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED);
        DrawRectangleLines(8,y,200,32,WHITE);
        DrawText(TextFormat("HP %.0f/%.0f",_hp,_maxHp),14,y+6,sz,WHITE);
        if(_hasShield){float sr=_shieldMax>0?_shieldHp/_shieldMax:0;DrawRectangle(8,y-18,200,12,DARKGRAY);DrawRectangle(8,y-18,(int)(200*sr),12,{60,120,255,255});DrawRectangleLines(8,y-18,200,12,{100,180,255,255});DrawText(TextFormat("SHIELD %.0f",_shieldHp),10,y-16,10,{150,200,255,255});}
        DrawLine(216,sh-58,216,sh,{40,40,65,255});
        int ax=224;
        DrawText(TextFormat("[%s]",_wname.c_str()),ax,y,sz,_reloading?ORANGE:YELLOW);
        int ww=MeasureText(TextFormat("[%s]",_wname.c_str()),sz);
        DrawText(TextFormat("%d/%d",_ammo,_maxAmmo),ax+ww+6,y,sz,WHITE);
        if(_reloading)DrawText("RECHARGEMENT...",ax,y-18,13,ORANGE);
        DrawText(TextFormat("2:[%s]",_w2name.c_str()),ax,y+sz+1,13,GRAY);
        DrawLine(ax+200,sh-58,ax+200,sh,{40,40,65,255});
        int vx=ax+208;
        DrawText(TextFormat("Vague %d/10",_wave_n),vx,y,sz,_wstate==WaveState::BETWEEN_WAVES?ORANGE:WHITE);
        if(_wstate==WaveState::BETWEEN_WAVES)DrawText(TextFormat("Boss dans %.0fs",_betweenT),vx,y-18,13,ORANGE);
        DrawLine(vx+160,sh-58,vx+160,sh,{40,40,65,255});
        int stx=vx+168;
        DrawText(TextFormat("Mat:%d",_mats),stx,y,sz,GREEN);
        DrawText(TextFormat("Kills:%d",_kills),stx+90,y,sz,ORANGE);
        DrawText(TextFormat("Vol:%d%%",(int)(_g->masterVolume*100)),sw-90,sh-58+10,13,{80,80,80,255});
        const char*hint="[R]Reload  [E]Pickup  [Scroll/Clic D]Swap  [Echap]Menu";
        int fsz=12;DrawText(hint,sw-MeasureText(hint,fsz)-8,sh-fsz-5,fsz,{60,60,70,255});
        Vector2 m=GetMousePosition();int cx=(int)m.x,cy=(int)m.y;
        Color cc={255,255,255,210};
        DrawLine(cx-14,cy,cx-5,cy,cc);DrawLine(cx+5,cy,cx+14,cy,cc);
        DrawLine(cx,cy-14,cx,cy-5,cc);DrawLine(cx,cy+5,cx,cy+14,cc);
        DrawCircleLines(cx,cy,5,{255,255,255,150});
    }
};
