#pragma once
#include "Engine.hpp"
#include "IGame.hpp"
#include <memory>
#include <cmath>
#include <string>
#include <algorithm>

std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

// ================================================================
// INDEX SONS SFX (dans _allMusics[])
// ================================================================
namespace SFX {
    static const int PISTOL     = 0;
    static const int SHOTGUN    = 1;
    static const int SNIPER     = 2;
    static const int AK47       = 3;
    static const int ROCKET     = 4;
    static const int ENEMY_DIE  = 5;
    static const int PICKUP     = 6;
    static const int RELOAD     = 7;
    static const int UI_CLICK   = 8;
    static const int UI_CONFIRM = 9;
    static const int VOL_TICK   = 10;
    static const int BRIGHT_TICK= 11;
}

// ================================================================
// SPLASH SCENE — Cinématique : Activision -> TiMi -> Loading -> Menu
// ================================================================
class SplashScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;

    enum class Phase { ACTIVISION, TIMI, LOADING };
    Phase _phase     = Phase::ACTIVISION;
    float _timer     = 0.f;
    float _alpha     = 0.f;   // 0=noir 1=pleine image
    float _loadPct   = 0.f;

    static constexpr float FADE_IN  = 0.3f;
    static constexpr float HOLD     = 1.4f;    // 2s total par logo
    static constexpr float FADE_OUT = 0.3f;
    static constexpr float LOGO_DUR = FADE_IN + HOLD + FADE_OUT; // ~2s

    // Texture indices
    static const int TEX_ACT  = 32;
    static const int TEX_TIMI = 33;

public:
    SplashScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Splash"), _sm(&sm), _g(&g) {}

    void onEnter() override {
        _phase=Phase::ACTIVISION; _timer=0; _alpha=0; _loadPct=0;
        // Musique démarre IMMÉDIATEMENT à l'ouverture de la fenêtre
        _g->playBGMusic("assets/sounds/music_menu.ogg");
    }
    void onExit() override {}

    void update(float dt) override {
        _g->updateBGMusic();
        _timer += dt;

        // Skip avec Espace/Entrée/clic
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (_phase != Phase::LOADING) {
                _phase = Phase::LOADING; _timer = 0; _loadPct = 0;
            }
        }

        switch (_phase) {
        case Phase::ACTIVISION:
        case Phase::TIMI: {
            if      (_timer < FADE_IN)            _alpha = _timer / FADE_IN;
            else if (_timer < FADE_IN + HOLD)     _alpha = 1.f;
            else if (_timer < LOGO_DUR)           _alpha = 1.f - (_timer - FADE_IN - HOLD) / FADE_OUT;
            else {
                _alpha = 0.f;
                if (_phase == Phase::ACTIVISION) { _phase = Phase::TIMI;    _timer = 0; }
                else                             { _phase = Phase::LOADING; _timer = 0; _loadPct = 0; }
            }
            break;
        }
        case Phase::LOADING: {
            _loadPct = std::min(1.f, _timer / 10.0f);  // 10 secondes
            if (_loadPct >= 1.f && _timer > 11.f)
                _sm->changeScene(SceneType::MENU);
            break;
        }
        }
    }

    void render(float dt) override {
        (void)dt;
        int sw = GetScreenWidth(), sh = GetScreenHeight();

        if (_phase == Phase::ACTIVISION || _phase == Phase::TIMI) {
            DrawRectangle(0, 0, sw, sh, BLACK);
            int idx = (_phase == Phase::ACTIVISION) ? TEX_ACT : TEX_TIMI;
            if (idx < (int)_g->_allSprites.size()) {
                Texture2D& tex = _g->_allSprites[idx];
                float scX=(float)sw/tex.width, scY=(float)sh/tex.height;
                float sc=std::min(scX, scY);
                float dw=tex.width*sc, dh=tex.height*sc;
                float ox=(sw-dw)/2.f, oy=(sh-dh)/2.f;
                unsigned char a = (unsigned char)(_alpha * 255);
                DrawTexturePro(tex,
                    {0,0,(float)tex.width,(float)tex.height},
                    {ox,oy,dw,dh}, {0,0}, 0.f, {255,255,255,a});
            }
        } else {
            // ── Loading screen ────────────────────────────────
            // Fond = image brotato_menu (TEX_BG = 34) avec overlay sombre
            static const int TEX_MENU_BG = 34;
            DrawRectangle(0, 0, sw, sh, {8, 8, 18, 255});
            if (TEX_MENU_BG < (int)_g->_allSprites.size()) {
                Texture2D& bg = _g->_allSprites[TEX_MENU_BG];
                float sc2 = std::max((float)sw/bg.width, (float)sh/bg.height);
                float dw=bg.width*sc2, dh=bg.height*sc2;
                DrawTexturePro(bg,{0,0,(float)bg.width,(float)bg.height},
                               {(sw-dw)/2,(sh-dh)/2,dw,dh},{0,0},0.f,WHITE);
                // Overlay sombre pour lisibilité
                DrawRectangle(0,0,sw,sh,{0,0,0,160});
            }

            // Sous-titre discret (pas de ZIMEX animé ici)
            const char* sub = "Chargement en cours...";
            int ssz = sh/28;
            DrawText(sub, sw/2-MeasureText(sub,ssz)/2, (int)(sh*0.60f), ssz, {100,100,100,255});

            // Barre pixel style (vert néon sur fond bleu-nuit)
            int barW=(int)(sw*0.55f), barH=(int)(sh*0.05f);
            int barX=sw/2-barW/2,    barY=(int)(sh*0.68f);
            int border=4;

            // Fond bleu nuit
            DrawRectangle(barX,barY,barW,barH,{18,16,38,255});
            // Remplissage vert
            int fill=(int)(barW*_loadPct);
            if (fill > border*2) {
                DrawRectangle(barX+border, barY+border, fill-border*2, barH-border*2, {100,200,20,255});
                // Reflet haut
                int hi=std::max(2,barH/6);
                DrawRectangle(barX+border, barY+border, fill-border*2, hi, {150,255,30,200});
                // Ombre bas
                DrawRectangle(barX+border, barY+barH-border*2, fill-border*2, border, {55,130,8,200});
                // Segments verticaux
                for (int sx=barX+border; sx<barX+fill-8; sx+=12)
                    DrawRectangle(sx, barY+border, 2, barH-border*2, {0,0,0,28});
            }
            // Bordure
            DrawRectangleLines(barX, barY, barW, barH, {50,55,70,255});
            // Pourcentage
            std::string pct = std::to_string((int)(_loadPct*100)) + "%";
            int psz = barH*6/10;
            DrawText(pct.c_str(), barX+barW+10, barY+(barH-psz)/2, psz, {100,200,20,255});

            if (_loadPct >= 1.f) {
                float pulse = 0.5f + 0.5f*std::sin(GetTime()*4.f);
                const char* rdy = "Appuie sur ESPACE pour continuer";
                int rsz=sh/32;
                DrawText(rdy, sw/2-MeasureText(rdy,rsz)/2, (int)(sh*0.82f),
                         rsz, {255,255,255,(unsigned char)(pulse*220)});
            }
        }
    }
};

// ================================================================
// INFO SCENE
// ================================================================
class InfoScene : public Scene {
    SceneManager* _sm;
    int _page = 0;
    float _anim = 0;
public:
    InfoScene(SceneManager& sm):Scene("Info"),_sm(&sm){}
    void onEnter() override { _page=0; _anim=0; }
    void onExit()  override {}
    void update(float dt) override {
        _anim+=dt;
        if (IsKeyPressed(KEY_ESCAPE)) { _sm->changeScene(SceneType::MENU); return; }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            float ny=GetMousePosition().y/(float)GetScreenHeight();
            float nx=GetMousePosition().x/(float)GetScreenWidth();
            if (ny>=0.84f&&ny<=0.91f&&nx>=0.33f&&nx<=0.67f) { _sm->changeScene(SceneType::MENU); return; }
            if (_page==0&&nx>=0.60f&&nx<=0.90f&&ny>=0.84f) { _page=1; return; }
            if (_page==1&&nx>=0.10f&&nx<=0.40f&&ny>=0.84f) { _page=0; return; }
        }
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(),sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for(int y=0;y<sh;y+=sh/20) DrawLine(0,y,sw,y,{10,10,28,255});
        int tsz=sh/10; const char*t="AIDE & CONTRÔLES";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+2,sh/14+2,tsz,{0,0,0,150});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/14,  tsz,WHITE);
        int lsz=sh/22, lx=(int)(sw*0.07f), ly=(int)(sh*0.22f), ls=(int)(lsz*1.5f);
        if (_page==0) {
            struct Row{const char*lbl;const char*val;Color c;};
            Row rows[]={
                {"WASD / Flèches","Déplacer le joueur",WHITE},
                {"Souris","Viser",{180,220,255,255}},
                {"Clic Gauche","Tirer",{255,200,80,255}},
                {"Clic Droit / Scroll","Changer d'arme",{200,255,200,255}},
                {"R","Recharger",{255,160,80,255}},
                {"E","Ramasser une arme",{80,255,160,255}},
                {"Espace","Pause/Skip",GRAY},
                {"Échap","Retour au menu",GRAY},
            };
            for (auto& r:rows) {
                DrawText(r.lbl,lx,ly,lsz,r.c);
                DrawText(r.val,lx+(int)(sw*0.35f),ly,lsz,DARKGRAY);
                ly+=ls;
            }
        } else {
            struct Row{const char*lbl;Color c;};
            Row rows[]={
                {"Survive 10 vagues de monstres !",YELLOW},
                {"Chaque vague se termine par un BOSS unique.",{255,160,60,255}},
                {"Ramasse les orbes verts = XP/Matériaux.",{100,255,100,255}},
                {"Ramasse les objets dorés = munitions.",{255,220,60,255}},
                {"Ramasse les orbes bleus = soins +50HP.",{80,180,255,255}},
                {"Ramasse les boucliers violets.",{180,120,255,255}},
                {"Equipe des armes au sol (touche E).",{200,200,200,255}},
                {"Clique droit pour changer d'arme.",WHITE},
            };
            for (auto& r:rows) { DrawText(r.lbl,lx,ly,lsz,r.c); ly+=ls; }
        }
        // Pagination
        int by=(int)(sh*0.84f),bh=(int)(sh*0.07f),bw=(int)(sw*0.25f);
        if (_page==0) {
            DrawRectangle(sw-bw-(int)(sw*0.06f),by,bw,bh,{20,60,20,220});
            DrawRectangleLines(sw-bw-(int)(sw*0.06f),by,bw,bh,{60,120,60,255});
            const char*nx2="Suite >";int nsz=bh*6/10;
            DrawText(nx2,sw-bw-(int)(sw*0.06f)+bw/2-MeasureText(nx2,nsz)/2,by+bh/2-nsz/2,nsz,GREEN);
        } else {
            DrawRectangle((int)(sw*0.06f),by,bw,bh,{20,20,60,220});
            DrawRectangleLines((int)(sw*0.06f),by,bw,bh,{60,60,120,255});
            const char*pv="< Préc";int psz=bh*6/10;
            DrawText(pv,(int)(sw*0.06f)+bw/2-MeasureText(pv,psz)/2,by+bh/2-psz/2,psz,SKYBLUE);
        }
        int rby=by,rbw=(int)(sw*0.28f),rbx=sw/2-rbw/2;
        DrawRectangle(rbx,rby,rbw,bh,{15,15,60,220});
        DrawRectangleLines(rbx,rby,rbw,bh,{50,50,120,255});
        const char*bk="< Retour Menu";int bksz=bh*6/10;
        DrawText(bk,rbx+rbw/2-MeasureText(bk,bksz)/2,rby+bh/2-bksz/2,bksz,{100,200,255,255});
        // Page indicator
        DrawText(TextFormat("Page %d/2",_page+1),sw/2-20,by-lsz,lsz-4,GRAY);
    }
};

// ================================================================
// MENU SCENE — fond Brotato + musique de fond
// ================================================================
class MenuScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    float _anim = 0;
    static const int TEX_BG = 34;
public:
    MenuScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Menu"), _sm(&sm), _g(&g) {}

    void onEnter() override {
        _anim = 0;
        _g->playBGMusic("assets/sounds/music_menu.ogg");
    }
    void onExit() override {}

    void update(float dt) override {
        _g->updateBGMusic();
        _anim += dt;

        // Volume +/-
        if (IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD)) {
            _g->masterVolume = std::min(1.f, _g->masterVolume+0.05f);
            SetMasterVolume(_g->masterVolume);
            _g->playSFX(SFX::VOL_TICK);
        }
        if (IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT)) {
            _g->masterVolume = std::max(0.f, _g->masterVolume-0.05f);
            SetMasterVolume(_g->masterVolume);
            _g->playSFX(SFX::VOL_TICK);
        }

        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
        Vector2 m = GetMousePosition();
        float nx=m.x/GetScreenWidth(), ny=m.y/GetScreenHeight();

        if (nx>=0.33f&&nx<=0.67f&&ny>=0.36f&&ny<=0.43f) {
            _g->playSFX(SFX::UI_CONFIRM);
            _sm->changeScene(SceneType::GAME);
        }
        else if (nx>=0.33f&&nx<=0.67f&&ny>=0.46f&&ny<=0.53f) {
            _g->playSFX(SFX::UI_CLICK);
            _sm->changeScene(SceneType::INFO);
        }
        else if (nx>=0.33f&&nx<=0.67f&&ny>=0.56f&&ny<=0.63f) {
            _g->playSFX(SFX::UI_CLICK);
            _sm->changeScene(SceneType::SETTINGS);
        }
        else if (nx>=0.33f&&nx<=0.67f&&ny>=0.66f&&ny<=0.73f) {
            _g->playSFX(SFX::UI_CLICK);
            CloseWindow();
        }
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();

        // Fond Brotato
        if (TEX_BG < (int)_g->_allSprites.size()) {
            Texture2D& bg = _g->_allSprites[TEX_BG];
            float sc = std::max((float)sw/bg.width, (float)sh/bg.height);
            float dw=bg.width*sc, dh=bg.height*sc;
            DrawTexturePro(bg,{0,0,(float)bg.width,(float)bg.height},
                           {(sw-dw)/2,(sh-dh)/2,dw,dh},{0,0},0.f,WHITE);
            DrawRectangle(0,0,sw,sh,{0,0,0,145});
        } else {
            DrawRectangle(0,0,sw,sh,{5,5,15,255});
        }

        // Titre ZIMEX avec effet pulsant
        float pulse = 1.f + 0.04f*std::sin(_anim*3.f);
        int tsz=(int)(sh/7*pulse);
        const char* title = "ZIMEX";
        DrawText(title, sw/2-MeasureText(title,tsz)/2+4, sh/10+4, tsz, {0,0,0,200});
        DrawText(title, sw/2-MeasureText(title,tsz)/2+2, sh/10+2, tsz, {80,40,0,180});
        DrawText(title, sw/2-MeasureText(title,tsz)/2,   sh/10,   tsz, YELLOW);

        // Boutons
        struct Btn { float y1,y2; const char* lbl; Color bg; Color fg; };
        Btn btns[] = {
            {0.36f,0.43f, "> Jouer",         {20,80,20,230},  GREEN},
            {0.46f,0.53f, "> Aide / Infos",  {15,45,90,230},  {100,200,255,255}},
            {0.56f,0.63f, "> Paramètres",    {15,15,70,230},  WHITE},
            {0.66f,0.73f, "> Quitter",       {80,15,15,230},  RED},
        };
        Vector2 mouse = GetMousePosition();
        for (auto& b : btns) {
            int bx=(int)(sw*0.33f), by=(int)(sh*b.y1);
            int bw=(int)(sw*0.34f), bh=(int)(sh*(b.y2-b.y1));
            bool hover = (mouse.x>=bx&&mouse.x<=bx+bw&&mouse.y>=by&&mouse.y<=by+bh);
            // Ombre
            DrawRectangle(bx+3,by+3,bw,bh,{0,0,0,120});
            Color bg = b.bg;
            if (hover) bg = {(unsigned char)std::min(255,bg.r+30),(unsigned char)std::min(255,bg.g+30),(unsigned char)std::min(255,bg.b+30),255};
            DrawRectangle(bx,by,bw,bh,bg);
            DrawRectangleLines(bx,by,bw,bh,{120,120,140,220});
            int lsz=bh*6/10;
            DrawText(b.lbl, bx+14, by+bh/2-lsz/2, lsz, b.fg);
        }

        // Volume affiché en bas
        int vsz=sh/32;
        DrawText(TextFormat("Vol:%d%%  (+/-)",(int)(_g->masterVolume*100)),
                 (int)(sw*0.02f), sh-vsz*2, vsz, {140,140,140,255});
    }
};

// ================================================================
// SETTINGS SCENE — volume/luminosité/plein écran avec sons
// ================================================================
class SettingsScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;
    bool  _draggingVol   = false;
    bool  _draggingBright= false;
    float _prevVol       = 0.8f;
    float _prevBright    = 1.0f;
public:
    SettingsScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Settings"), _sm(&sm), _g(&g) {}

    void onEnter() override {
        _prevVol    = _g->masterVolume;
        _prevBright = _g->brightness;
    }
    void onExit() override {}

    void update(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        int lsz=sh/20;

        // ── Slider VOLUME ─────────────────────────────────
        int vbx=(int)(sw*0.44f), vby=(int)(sh*0.28f), vbw=(int)(sw*0.42f), vbh=lsz;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m=GetMousePosition();
            if (m.x>=vbx&&m.x<=vbx+vbw&&m.y>=vby&&m.y<=vby+vbh)
                _draggingVol=true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) _draggingVol=false;
        if (_draggingVol) {
            float nv=std::max(0.f,std::min(1.f,(GetMousePosition().x-vbx)/(float)vbw));
            _g->masterVolume=nv;
            SetMasterVolume(nv);
            // Son tick si changement significatif
            if (std::fabs(nv-_prevVol)>0.04f) {
                _g->playSFX(SFX::VOL_TICK);
                _prevVol=nv;
            }
        }
        // Touches +/-
        if (IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD)) {
            _g->masterVolume=std::min(1.f,_g->masterVolume+0.05f);
            SetMasterVolume(_g->masterVolume);
            _g->playSFX(SFX::VOL_TICK);
        }
        if (IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT)) {
            _g->masterVolume=std::max(0.f,_g->masterVolume-0.05f);
            SetMasterVolume(_g->masterVolume);
            _g->playSFX(SFX::VOL_TICK);
        }

        // ── Slider LUMINOSITÉ ─────────────────────────────
        int bby=(int)(sh*0.28f)+lsz+(int)(lsz*1.5f);
        int bbx=vbx, bbw=vbw, bbh=lsz;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m=GetMousePosition();
            if (m.x>=bbx&&m.x<=bbx+bbw&&m.y>=bby&&m.y<=bby+bbh)
                _draggingBright=true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) _draggingBright=false;
        if (_draggingBright) {
            float nb=std::max(0.1f,std::min(1.f,(GetMousePosition().x-bbx)/(float)bbw));
            _g->brightness=nb;
            if (std::fabs(nb-_prevBright)>0.04f) {
                _g->playSFX(SFX::BRIGHT_TICK);
                _prevBright=nb;
            }
        }
        // Touches flèches haut/bas
        if (IsKeyPressed(KEY_UP)) {
            _g->brightness=std::min(1.f,_g->brightness+0.05f);
            _g->playSFX(SFX::BRIGHT_TICK);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            _g->brightness=std::max(0.1f,_g->brightness-0.05f);
            _g->playSFX(SFX::BRIGHT_TICK);
        }

        // Fullscreen F11
        if (IsKeyPressed(KEY_F11)) {
            _g->isFullscreen=!_g->isFullscreen;
            if (_g->isFullscreen) {
                int mon=GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(mon),GetMonitorHeight(mon));
                ToggleFullscreen();
            } else {
                ToggleFullscreen();
                SetWindowSize(1280,720);
            }
            _g->playSFX(SFX::UI_CLICK);
        }

        // Retour
        if (IsKeyPressed(KEY_ESCAPE)) {
            _g->playSFX(SFX::UI_CLICK);
            _sm->changeScene(SceneType::MENU);
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            float ny=GetMousePosition().y/(float)sh;
            float nx2=GetMousePosition().x/(float)sw;
            if (ny>=0.83f&&ny<=0.91f&&nx2>=0.33f&&nx2<=0.67f) {
                _g->playSFX(SFX::UI_CLICK);
                _sm->changeScene(SceneType::MENU);
            }
        }
    }

    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();

        // Fond avec luminosité appliquée
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        // Overlay luminosité : plus sombre si brightness basse
        float b = _g->brightness;
        if (b < 1.f) {
            unsigned char dark = (unsigned char)((1.f-b)*220.f);
            DrawRectangle(0,0,sw,sh,{0,0,0,dark});
        }

        int tsz=sh/9;
        const char* t="PARAMÈTRES";
        DrawText(t, sw/2-MeasureText(t,tsz)/2+2, sh/12+2, tsz, {0,0,0,120});
        DrawText(t, sw/2-MeasureText(t,tsz)/2,   sh/12,   tsz, WHITE);

        int lsz=sh/20, lx=(int)(sw*0.14f);
        int by=(int)(sh*0.28f), ls=(int)(lsz*1.5f);
        int bx=(int)(sw*0.44f), bw=(int)(sw*0.42f), bh=lsz;

        // ── Slider VOLUME ──────────────────────────────────
        DrawText("Volume",     lx, by, lsz, WHITE);
        DrawText("(glisse ou +/-)", lx+MeasureText("Volume",lsz)+10, by, lsz-6, DARKGRAY);
        DrawRectangle(bx, by, bw, bh, DARKGRAY);
        DrawRectangle(bx, by, (int)(bw*_g->masterVolume), bh, GREEN);
        int hx = bx+(int)(bw*_g->masterVolume);
        DrawRectangle(hx-5,by-5,10,bh+10,WHITE);
        DrawRectangleLines(bx,by,bw,bh,{100,120,100,255});
        DrawText(TextFormat("%d%%",(int)(_g->masterVolume*100)), bx+bw+10, by, lsz, GREEN);

        by += ls;

        // ── Slider LUMINOSITÉ ──────────────────────────────
        DrawText("Luminosité", lx, by, lsz, WHITE);
        DrawText("(glisse ou ↑↓)", lx+MeasureText("Luminosité",lsz)+10, by, lsz-6, DARKGRAY);
        DrawRectangle(bx, by, bw, bh, DARKGRAY);
        DrawRectangle(bx, by, (int)(bw*_g->brightness), bh, YELLOW);
        int bhx = bx+(int)(bw*_g->brightness);
        DrawRectangle(bhx-5,by-5,10,bh+10,WHITE);
        DrawRectangleLines(bx,by,bw,bh,{120,120,80,255});
        DrawText(TextFormat("%d%%",(int)(_g->brightness*100)), bx+bw+10, by, lsz, YELLOW);

        by += ls;

        // ── Fullscreen ─────────────────────────────────────
        bool fs = _g->isFullscreen;
        DrawText(TextFormat("Plein écran [F11] : %s", fs?"ON":"OFF"),
                 lx, by, lsz, fs?GREEN:GRAY);

        // ── Bouton retour ──────────────────────────────────
        int rby=(int)(sh*0.84f), rbh=(int)(sh*0.07f), rbw=(int)(sw*0.28f), rbx=sw/2-rbw/2;
        DrawRectangle(rbx,rby,rbw,rbh,{15,15,60,220});
        DrawRectangleLines(rbx,rby,rbw,rbh,{50,50,120,255});
        const char* bk="< Retour"; int bksz=rbh*6/10;
        DrawText(bk, rbx+rbw/2-MeasureText(bk,bksz)/2, rby+rbh/2-bksz/2, bksz, {100,200,255,255});
    }
};

// ================================================================
// GAME OVER SCENE
// ================================================================
class GameOverScene : public Scene {
    SceneManager* _sm; bool _victory; float _timer=0;
public:
    GameOverScene(SceneManager& sm, bool v=false)
        : Scene(v?"Victory":"GameOver"), _sm(&sm), _victory(v) {}
    void onEnter() override { _timer=0; }
    void onExit()  override {}
    void update(float dt) override {
        _timer+=dt;
        if (_timer>0.8f&&(IsKeyPressed(KEY_ENTER)||IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            _sm->changeScene(SceneType::MENU);
    }
    void render(float dt) override {
        (void)dt;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,0,sw,sh,{5,5,15,255});
        for (int y=0;y<sh;y+=4) DrawRectangle(0,y,sw,2,{0,0,0,40});
        int tsz=sh/7; const char* t=_victory?"VICTOIRE !":"GAME OVER";
        DrawText(t,sw/2-MeasureText(t,tsz)/2+4,sh/3+4,tsz,{0,0,0,200});
        DrawText(t,sw/2-MeasureText(t,tsz)/2,  sh/3,  tsz,_victory?YELLOW:RED);
        if (_victory) {
            const char* s="10 Vagues Complétées !"; int ssz=sh/16;
            DrawText(s,sw/2-MeasureText(s,ssz)/2,(int)(sh*0.55f),ssz,GREEN);
        }
        if (_timer>0.8f) {
            float a=std::min(1.f,(_timer-0.8f)/0.5f);
            const char* sub="Appuie sur ENTRÉE ou clique pour revenir au menu"; int ssz=sh/26;
            DrawText(sub,sw/2-MeasureText(sub,ssz)/2,(int)(sh*0.72f),ssz,{255,255,255,(unsigned char)(a*255)});
        }
    }
};

// ================================================================
// GAME SCENE — Map large 3840×2160, caméra, collisions obstacles
// ================================================================
class GameScene : public Scene {
    SceneManager*    _sm;
    GlobalComponent* _g;

    // Systèmes
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
    InputContext    _ctx;

    // Entités
    Entity _player{0}, _waveEnt{0};

    // ── Animation player ─────────────────────────────────
    static const int FW=48, FH=64, N_FRAMES=6;
    static int weaponTex(const std::string& t) {
        if(t=="pistol")  return 1; if(t=="shotgun") return 2;
        if(t=="sniper")  return 3; if(t=="ak47")    return 4;
        if(t=="bomb")    return 5; return 1;
    }

    enum class Dir { DOWN=0, UP=1, RIGHT=2, LEFT=3 };
    Dir   _dir=Dir::DOWN;
    int   _frame=0;
    float _animT=0.f, _animFPS=9.f;
    bool  _moving=false;

    std::string _carriedType="pistol";
    float       _aimAngle=0.f;

    // ── Caméra ───────────────────────────────────────────
    // Le viewport virtuel est 1920×1080
    // La caméra déplace la vue dans la world map 3840×2160
    float _camX=0.f, _camY=0.f;  // coin haut-gauche de la vue dans le monde

    // ── HUD cache ────────────────────────────────────────
    float  _hp=200,_maxHp=200,_shieldHp=0,_shieldMax=360;
    bool   _hasShield=false;
    int    _ammo=0,_maxAmmo=0,_kills=0,_mats=0,_wave_n=1;
    bool   _reloading=false;
    std::string _wname="Pistol",_w2name="Shotgun";
    WaveState   _wstate=WaveState::FIGHTING;
    float       _betweenT=0;

    // Annonce de vague
    float _waveAnnounceTimer = 0.f;
    int   _lastAnnouncedWave = 0;

public:
    GameScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Game"), _sm(&sm), _g(&g) {}

    // ── Init ─────────────────────────────────────────────
    void initSystems() {
        auto W=[&](System& s){ s.setWorld(world); };
        W(_render); _render.setGlobal(*_g); _render.initRenderTarget();
        W(_input);  W(_collision); W(_physics); W(_health);
        W(_animation); _animation.setGlobal(*_g);
        W(_weapon);    _weapon.setGlobal(*_g);
        W(_enemy);     _enemy.setGlobal(*_g); _enemy.setSceneManager(*_sm);
        W(_bullet);
        W(_wave);      _wave.setGlobal(*_g);  _wave.setSceneManager(*_sm);
        W(_sound);     _sound.setGlobal(*_g);

        // Bindings WASD — mouvement uniquement
        auto mv=[](float& axis, float d){ return [&axis,d](World& w, Entity e){
            if(auto*p=w.getComponent<PositionComponent>(e))
                if(auto*pl=w.getComponent<PlayerComponent>(e))
                    axis+=d*pl->speed*GetFrameTime();
        }; };
        // On utilise une approche différente pour avoir accès à la position
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

    // ── Spawn MAP (fond + obstacles visuels) ─────────────
    void spawnMap() {
        // Fond de la map (texture game_map.png tirée comme background)
        Entity bg=world.createEntity();
        PositionComponent p; p.x=0; p.y=0; world.addComponent(bg,p);
        SpriteComponent s;
        s.texturePath=36;   // game_map.png [36]
        s.width=(float)MAP_WORLD_W; s.height=(float)MAP_WORLD_H;
        s.scale=1.f; s.offsetX=0; s.offsetY=0;
        s.layer=0; s.tag=EntityTag::NONE;
        world.addComponent(bg,s);

        // Obstacles invisibles (WALL) pour les collisions
        for (auto& obs : MAP_OBSTACLES) {
            Entity wall=world.createEntity();
            PositionComponent wp; wp.x=obs.x; wp.y=obs.y; world.addComponent(wall,wp);
            RectangleComponent wr; wr.width=(int)obs.width; wr.height=(int)obs.height;
            wr.inlineColor={0,0,0,0}; wr.tickness=0; wr.layer=-1;
            world.addComponent(wall,wr);
            BoxColliderComponent wc; wc.isCircle=false;
            wc.TagsCollided={(int)EntityTag::PLAYER,(int)EntityTag::ENEMY,(int)EntityTag::BOSS};
            world.addComponent(wall,wc);
        }
    }

    void spawnPlayer() {
        _player=world.createEntity();
        PositionComponent pos; pos.x=SPAWN_CENTER_X; pos.y=SPAWN_CENTER_Y;
        world.addComponent(_player,pos);
        HealthComponent hp; hp.hp=hp.maxHp=200.f; world.addComponent(_player,hp);
        SpriteComponent spr;
        spr.texturePath=weaponTex("pistol");
        spr.left=0.f; spr.top=0.f;
        spr.width=(float)FW; spr.height=(float)FH;
        spr.scale=2.5f;
        spr.offsetX=-(float)FW*spr.scale/2.f; spr.offsetY=-(float)FH*spr.scale/2.f;
        spr.tag=EntityTag::PLAYER; spr.layer=2; spr.rotation=0.f;
        world.addComponent(_player,spr);
        BoxColliderComponent col; col.isCircle=true; col.radius=20.f;
        col.TagsCollided={(int)EntityTag::BULLET_EN,(int)EntityTag::PICKUP,
                          (int)EntityTag::XP_ORB,(int)EntityTag::ENEMY,(int)EntityTag::BOSS,
                          (int)EntityTag::PICKUP_AMMO,(int)EntityTag::PICKUP_HP,
                          (int)EntityTag::PICKUP_SHIELD};
        world.addComponent(_player,col);
        world.addComponent(_player,InputComponent{});
        PlayerComponent pl; pl.speed=220.f; world.addComponent(_player,pl);
        WeaponComponent wc;
        wc.slots[0]={createWeapon("pistol",&world),"pistol"};
        wc.slots[1]={createWeapon("shotgun",&world),"shotgun"};
        wc.activeSlot=0;
        world.addComponent(_player,wc);
        // Caméra centrée sur le player au départ
        _camX=SPAWN_CENTER_X-960.f;
        _camY=SPAWN_CENTER_Y-540.f;
        _dir=Dir::DOWN; _frame=0; _animT=0; _moving=false;
        _carriedType="pistol"; _aimAngle=0.f;
    }

    void spawnWave() {
        _waveEnt=world.createEntity();
        PositionComponent p; world.addComponent(_waveEnt,p);
        WaveComponent wc; wc.currentWave=1; wc.maxWaves=6;
        wc.enemiesLeft=10; wc.state=WaveState::FIGHTING;
        wc.spawnRate=1.2f; wc.betweenMax=4.f;
        world.addComponent(_waveEnt,wc);
    }

    void spawnPickup(float wx, float wy, const std::string& type, int tex) {
        Entity e=world.createEntity();
        PositionComponent p; p.x=wx; p.y=wy; world.addComponent(e,p);
        SpriteComponent s; s.texturePath=tex; s.width=24; s.height=24;
        s.scale=2.f; s.offsetX=-24; s.offsetY=-24;
        s.tag=EntityTag::PICKUP; s.layer=1;
        world.addComponent(e,s);
        BoxColliderComponent c; c.isCircle=true; c.radius=24.f;
        c.TagsCollided={(int)EntityTag::PLAYER};
        world.addComponent(e,c);
        WeaponComponent wc; wc.isPickup=true; wc.pickupWeaponType=type;
        world.addComponent(e,wc);
    }

    void onEnter() override {
        initSystems(); spawnMap(); spawnPlayer(); spawnWave();
        // Pickups éparpillés dans le monde
        spawnPickup(SPAWN_CENTER_X-800, SPAWN_CENTER_Y-400, "sniper",  12);
        spawnPickup(SPAWN_CENTER_X+800, SPAWN_CENTER_Y+400, "bomb",    13);
        spawnPickup(SPAWN_CENTER_X+800, SPAWN_CENTER_Y-400, "ak47",    11);
        spawnPickup(SPAWN_CENTER_X-600, SPAWN_CENTER_Y+600, "shotgun", 11);
        // Musique de jeu
        _g->playBGMusic("assets/sounds/music_game.ogg");
    }
    void onExit() override {
        world.clearWorld(); _ctx.clear();
    }

    // ── Camera update ─────────────────────────────────────
    void updateCamera() {
        auto* pos=world.getComponent<PositionComponent>(_player);
        if (!pos) return;
        // Centre la caméra sur le player, clamped aux bords de la map
        _camX=pos->x-960.f;
        _camY=pos->y-540.f;
        _camX=std::max(0.f, std::min(_camX, (float)(MAP_WORLD_W-1920)));
        _camY=std::max(0.f, std::min(_camY, (float)(MAP_WORLD_H-1080)));
    }

    // ── Obstacle collision (push-out) ──────────────────────
    void resolveObstacles() {
        auto* pos=world.getComponent<PositionComponent>(_player);
        auto* col=world.getComponent<BoxColliderComponent>(_player);
        if (!pos||!col) return;
        float r=col->radius;
        for (auto& obs : MAP_OBSTACLES) {
            // AABB vs circle push-out
            float cx=pos->x, cy=pos->y;
            float rx=obs.x, ry=obs.y, rw=obs.width, rh=obs.height;
            // Closest point on rect
            float nearX=std::max(rx,std::min(cx,rx+rw));
            float nearY=std::max(ry,std::min(cy,ry+rh));
            float dx=cx-nearX, dy=cy-nearY;
            float dist2=dx*dx+dy*dy;
            if (dist2>0 && dist2<r*r) {
                float dist=std::sqrt(dist2);
                float push=r-dist;
                pos->x+=dx/dist*push;
                pos->y+=dy/dist*push;
            } else if (dist2==0) {
                // Player est dans le rect — pousser vers le haut
                pos->y-=r;
            }
        }
        // Clamper aux bords du monde
        pos->x=std::max(30.f, std::min(pos->x,(float)(MAP_WORLD_W-30)));
        pos->y=std::max(30.f, std::min(pos->y,(float)(MAP_WORLD_H-30)));
    }

    // ── Enemy obstacle avoidance ──────────────────────────
    void resolveEnemyObstacles() {
        for (Entity e : world.getContainer<EnemyAIComponent>().getEntities()) {
            auto* pos=world.getComponent<PositionComponent>(e);
            auto* col=world.getComponent<BoxColliderComponent>(e);
            if (!pos||!col) continue;
            float r=col->radius*0.8f; // un peu moins strict pour les ennemis
            for (auto& obs : MAP_OBSTACLES) {
                float cx=pos->x, cy=pos->y;
                float nearX=std::max(obs.x,std::min(cx,obs.x+obs.width));
                float nearY=std::max(obs.y,std::min(cy,obs.y+obs.height));
                float dx=cx-nearX, dy=cy-nearY;
                float dist2=dx*dx+dy*dy;
                if (dist2>0 && dist2<r*r) {
                    float dist=std::sqrt(dist2);
                    float push=r-dist;
                    pos->x+=dx/dist*push;
                    pos->y+=dy/dist*push;
                }
            }
            pos->x=std::max(10.f,std::min(pos->x,(float)(MAP_WORLD_W-10)));
            pos->y=std::max(10.f,std::min(pos->y,(float)(MAP_WORLD_H-10)));
        }
    }

    // ── Animation sprite ──────────────────────────────────
    void tickAnimation(float dt) {
        bool w=IsKeyDown(KEY_W)||IsKeyDown(KEY_UP);
        bool s=IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN);
        bool a=IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT);
        bool d=IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT);
        _moving=w||s||a||d;

        Dir newDir=_dir;
        if (_moving) {
            if      (d&&!a)  newDir=Dir::RIGHT;
            else if (a&&!d)  newDir=Dir::LEFT;
            else if (w&&!s)  newDir=Dir::UP;
            else if (s&&!w)  newDir=Dir::DOWN;
            else if (d)      newDir=Dir::RIGHT;
            else if (a)      newDir=Dir::LEFT;
        } else {
            // Idle : oriente vers la souris (en coordonnées écran)
            float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
            float ox=(GetScreenWidth()-1920.f*sc)/2.f;
            float oy=(GetScreenHeight()-1080.f*sc)/2.f;
            auto* pos=world.getComponent<PositionComponent>(_player);
            if (pos) {
                // Position du player dans l'espace virtuel (décalée par caméra)
                float screenPX = ox + (pos->x-_camX)*sc;
                float screenPY = oy + (pos->y-_camY)*sc;
                float ddx=GetMousePosition().x-screenPX;
                float ddy=GetMousePosition().y-screenPY;
                if (std::fabs(ddx)>std::fabs(ddy)) newDir=(ddx>=0?Dir::RIGHT:Dir::LEFT);
                else                                newDir=(ddy>=0?Dir::DOWN:Dir::UP);
            }
        }
        if (newDir!=_dir){ _dir=newDir; _frame=0; _animT=0.f; }
        if (_moving){ _animT+=dt; if(_animT>=1.f/_animFPS){_animT=0.f;_frame=(_frame+1)%N_FRAMES;} }
        else { _frame=0; _animT=0.f; }

        if (auto* spr=world.getComponent<SpriteComponent>(_player)) {
            spr->left=(float)_frame*FW;
            spr->top=(float)_dir*FH;
            int newTex=weaponTex(_carriedType);
            if (spr->texturePath!=newTex) spr->texturePath=newTex;
        }
    }

    // ── Visée souris → angle dans le monde ────────────────
    void updateAim() {
        auto* pos=world.getComponent<PositionComponent>(_player);
        if (!pos) return;
        float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
        float ox=(GetScreenWidth()-1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;
        // Convertir position player (monde) → écran
        float px=ox+(pos->x-_camX)*sc;
        float py=oy+(pos->y-_camY)*sc;
        _aimAngle=std::atan2(GetMousePosition().y-py, GetMousePosition().x-px);
    }

    void update(float dt) override {
        _g->updateBGMusic();

        if (IsKeyPressed(KEY_ESCAPE)) {
            _g->playSFX(SFX::UI_CLICK);
            _sm->changeScene(SceneType::MENU);
            return;
        }
        // Volume jeu
        if (IsKeyPressed(KEY_EQUAL)||IsKeyPressed(KEY_KP_ADD)) {
            _g->masterVolume=std::min(1.f,_g->masterVolume+0.05f);
            SetMasterVolume(_g->masterVolume); _g->playSFX(SFX::VOL_TICK);
        }
        if (IsKeyPressed(KEY_MINUS)||IsKeyPressed(KEY_KP_SUBTRACT)) {
            _g->masterVolume=std::max(0.f,_g->masterVolume-0.05f);
            SetMasterVolume(_g->masterVolume); _g->playSFX(SFX::VOL_TICK);
        }
        if (IsKeyPressed(KEY_F11)) {
            _g->isFullscreen=!_g->isFullscreen;
            if(_g->isFullscreen){int m=GetCurrentMonitor();SetWindowSize(GetMonitorWidth(m),GetMonitorHeight(m));ToggleFullscreen();}
            else{ToggleFullscreen();SetWindowSize(1280,720);}
            _render.initRenderTarget();
        }

        // Mouvement player
        _input.update(dt);
        resolveObstacles();

        // Visée + tir
        updateAim();
        if (auto* pos=world.getComponent<PositionComponent>(_player)) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float tx=pos->x+std::cos(_aimAngle)*200.f;
                float ty=pos->y+std::sin(_aimAngle)*200.f;
                if (auto* wc=world.getComponent<WeaponComponent>(_player))
                    if (wc->current()) wc->current()->fire(pos->x,pos->y,tx,ty);
            }
        }

        // Swap arme
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)||GetMouseWheelMove()!=0) {
            if (auto* wc=world.getComponent<WeaponComponent>(_player)) {
                wc->swap(); _carriedType=wc->slots[wc->activeSlot].type;
            }
        }
        // Reload
        if (IsKeyPressed(KEY_R)) {
            if (auto* wc=world.getComponent<WeaponComponent>(_player))
                if (wc->current()) wc->current()->reload();
        }
        // Pickup arme au sol
        if (IsKeyPressed(KEY_E)) doPickupWeapon();

        // Systèmes
        _collision.update(dt);
        _bullet.update(dt);
        _enemy.update(dt);
        resolveEnemyObstacles();
        _wave.update(dt);
        _weapon.update(dt);
        _sound.update(dt);
        _animation.update(dt);

        handlePlayerDamage(dt);
        handleInvinc(dt);
        collectPickups();
        tickAnimation(dt);
        updateCamera();
        checkDeath();
        syncHUD();
        if (_waveAnnounceTimer > 0.f) _waveAnnounceTimer -= dt;
    }

    // ── Pickup arme ───────────────────────────────────────
    void doPickupWeapon() {
        auto* col=world.getComponent<BoxColliderComponent>(_player);
        if (!col) return;
        std::vector<Entity> rm;
        for (int id : col->EntityCollided) {
            Entity o=static_cast<Entity>(id);
            auto* spr=world.getComponent<SpriteComponent>(o);
            if (!spr||spr->tag!=EntityTag::PICKUP) continue;
            auto* pwc=world.getComponent<WeaponComponent>(o);
            if (!pwc||!pwc->isPickup) continue;
            auto* plwc=world.getComponent<WeaponComponent>(_player);
            if (plwc) {
                int slot=1-plwc->activeSlot;
                plwc->slots[slot]={createWeapon(pwc->pickupWeaponType,&world),pwc->pickupWeaponType};
                _carriedType=plwc->slots[plwc->activeSlot].type;
                _g->playSFX(SFX::PICKUP);
            }
            rm.push_back(o);
        }
        for (Entity e : rm) world.removeEntity(e);
    }

    // ── Collecte pickups HP/ammo/shield/XP ───────────────
    void collectPickups() {
        auto* col=world.getComponent<BoxColliderComponent>(_player);
        auto* pl=world.getComponent<PlayerComponent>(_player);
        auto* hp=world.getComponent<HealthComponent>(_player);
        auto* wc=world.getComponent<WeaponComponent>(_player);
        if (!col||!pl||!hp) return;
        std::vector<Entity> rm;
        for (int id : col->EntityCollided) {
            Entity o=static_cast<Entity>(id);
            auto* spr=world.getComponent<SpriteComponent>(o);
            if (!spr) continue;
            if (spr->tag==EntityTag::XP_ORB) {
                pl->materials++; rm.push_back(o);
                _g->playSFX(SFX::PICKUP, 0.5f);
            } else if (spr->tag==EntityTag::PICKUP_AMMO) {
                if (wc) for (auto& sl:wc->slots) if(sl.weapon) sl.weapon->refillAmmo();
                rm.push_back(o); _g->playSFX(SFX::PICKUP);
            } else if (spr->tag==EntityTag::PICKUP_HP) {
                hp->hp=std::min(hp->maxHp,hp->hp+50.f);
                rm.push_back(o); _g->playSFX(SFX::PICKUP);
            } else if (spr->tag==EntityTag::PICKUP_SHIELD) {
                pl->hasShield=true; pl->shieldHp=pl->shieldMax;
                rm.push_back(o); _g->playSFX(SFX::PICKUP);
            }
        }
        for (Entity e : rm) world.removeEntity(e);
    }

    // ── Dégâts player ─────────────────────────────────────
    void handlePlayerDamage(float dt) {
        auto* pl=world.getComponent<PlayerComponent>(_player);
        auto* col=world.getComponent<BoxColliderComponent>(_player);
        auto* hp=world.getComponent<HealthComponent>(_player);
        if (!pl||!col||!hp||pl->isInvincible) return;
        for (int id : col->EntityCollided) {
            Entity o=static_cast<Entity>(id);
            auto* spr=world.getComponent<SpriteComponent>(o);
            if (!spr) continue;
            float dmg=0; bool hit=false;
            if (spr->tag==EntityTag::BULLET_EN) {
                auto* b=world.getComponent<BulletComponent>(o);
                if (b) dmg=b->damage;
                world.removeEntity(o); hit=true;
            } else if (spr->tag==EntityTag::ENEMY||spr->tag==EntityTag::BOSS) {
                auto* ai=world.getComponent<EnemyAIComponent>(o);
                dmg=(ai?ai->damage:0.1f)*dt*2.f; hit=true;
            }
            if (hit&&dmg>0) {
                if (pl->hasShield&&pl->shieldHp>0) {
                    pl->shieldHp-=dmg;
                    if (pl->shieldHp<=0){ pl->shieldHp=0; pl->hasShield=false; }
                } else hp->hp-=dmg;
                pl->isInvincible=true; pl->invincTimer=0.5f;
                if (spr->tag==EntityTag::BULLET_EN) break;
            }
        }
    }

    void handleInvinc(float dt) {
        auto* pl=world.getComponent<PlayerComponent>(_player);
        if (!pl||!pl->isInvincible) return;
        pl->invincTimer-=dt;
        if (pl->invincTimer<=0.f){ pl->invincTimer=0; pl->isInvincible=false; }
        if (auto* spr=world.getComponent<SpriteComponent>(_player))
            spr->tint=(pl->isInvincible&&(int)(pl->invincTimer*12)%2==0)
                ? Color{255,80,80,160} : WHITE;
    }

    void checkDeath() {
        auto* hp=world.getComponent<HealthComponent>(_player);
        if (hp&&hp->hp<=0.f){ _sm->changeScene(SceneType::GAMEOVER); return; }
        auto* wv=world.getComponent<WaveComponent>(_waveEnt);
        if (wv&&wv->state==WaveState::VICTORY){ _sm->changeScene(SceneType::GAMEOVER); return; }
    }

    void syncHUD() {
        if (auto* hp=world.getComponent<HealthComponent>(_player)){ _hp=hp->hp; _maxHp=hp->maxHp; }
        if (auto* pl=world.getComponent<PlayerComponent>(_player)){
            _mats=pl->materials; _kills=pl->kills;
            _hasShield=pl->hasShield; _shieldHp=pl->shieldHp; _shieldMax=pl->shieldMax;
        }
        if (auto* wc=world.getComponent<WeaponComponent>(_player)){
            _ammo=wc->ammo(); _maxAmmo=wc->maxAmmo();
            _wname=wc->weaponName(); _reloading=wc->reloading();
            int ot=1-wc->activeSlot;
            _w2name=wc->slots[ot].weapon?wc->slots[ot].weapon->getName():"--";
            _carriedType=wc->slots[wc->activeSlot].type;
        }
        if (auto* wv=world.getComponent<WaveComponent>(_waveEnt)){
            _wave_n=wv->currentWave; _wstate=wv->state; _betweenT=wv->betweenTimer;
            // Déclencher l'annonce si nouvelle vague
            if (_wave_n != _lastAnnouncedWave && _wstate==WaveState::FIGHTING) {
                _lastAnnouncedWave = _wave_n;
                _waveAnnounceTimer = 3.5f;  // affiche pendant 3.5s
            }
        }
    }

    // ── RENDER ────────────────────────────────────────────
    // Le RenderSystem dessine les entités DANS le monde.
    // On applique la caméra via un offset sur toutes les positions lors du rendu.
    // Pour ça : on modifie l'offset de la RenderTexture.
    void render(float dt) override {
        // On passe l'offset caméra au RenderSystem
        _render.setCameraOffset(_camX, _camY);
        _render.update(dt);
        drawShield();
        drawEnemyHP();
        drawHUD();
        drawWaveAnnounce();
        drawCrosshair();
        applyBrightness();
    }

    void drawWaveAnnounce() {
        if (_waveAnnounceTimer <= 0.f) return;
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        float t = _waveAnnounceTimer;

        // Noms des vagues
        static const char* WAVE_NAMES[6] = {
            "VAGUE 1", "VAGUE 2", "VAGUE 3",
            "VAGUE 4", "VAGUE 5", "VAGUE FINALE"
        };
        static const char* BOSS_SUBTITLES[6] = {
            "Le Fusilleur Pourpre",
            "L'Oeil Blanc",
            "Le Bombardier",
            "La Fusée Rouge",
            "Le Maître des Ondes",
            "LE BOSS FINAL"
        };
        int idx = std::max(0, std::min(_wave_n-1, 5));
        const char* waveName = WAVE_NAMES[idx];
        const char* bossName = BOSS_SUBTITLES[idx];

        // Fade in/out : 0.4s entrée, tenu, 0.6s sortie
        float alpha = 1.f;
        if      (t > 3.1f)  alpha = (3.5f - t) / 0.4f;        // fade in
        else if (t < 0.6f)  alpha = t / 0.6f;                  // fade out
        alpha = std::max(0.f, std::min(1.f, alpha));

        // Clignotement rapide (3 fois/sec)
        bool blink = (int)(t * 3.f) % 2 == 0;

        // Taille pulsante
        float pulse = 1.f + 0.06f * std::sin(GetTime() * 8.f);
        int tsz = (int)(sh / 6 * pulse);
        if (_wave_n == 6) tsz = (int)(sh / 5 * pulse);  // boss final encore plus grand

        // Couleur : rouge pour vagues normales, or pour boss final
        Color mainColor = (_wave_n==6) ? Color{255,220,0,255} : Color{255,30,30,255};
        Color glowColor = (_wave_n==6) ? Color{255,160,0,255} : Color{200,0,0,255};

        if (blink || t < 1.5f) {  // clignote seulement les 2 premières secondes
            unsigned char a = (unsigned char)(alpha * 255);
            mainColor.a = a;
            glowColor.a = (unsigned char)(alpha * 160);

            // Halo sombre derrière
            int tw = MeasureText(waveName, tsz);
            int tx = sw/2 - tw/2;
            int ty = (int)(sh * 0.30f);

            // Ombre portée (décalage + flou simulé)
            for (int off=6; off>=1; off--) {
                Color shadow={0,0,0,(unsigned char)(alpha*80/off)};
                DrawText(waveName, tx+off, ty+off, tsz, shadow);
            }
            // Glow couleur
            DrawText(waveName, tx-2, ty,   tsz, glowColor);
            DrawText(waveName, tx+2, ty,   tsz, glowColor);
            DrawText(waveName, tx,   ty-2, tsz, glowColor);
            DrawText(waveName, tx,   ty+2, tsz, glowColor);
            // Texte principal
            DrawText(waveName, tx, ty, tsz, mainColor);

            // Sous-titre (nom du boss, plus petit)
            int ssz = sh/18;
            int sw2 = MeasureText(bossName, ssz);
            int sx = sw/2 - sw2/2;
            int sy = ty + tsz + 8;
            Color subCol = (_wave_n==6) ? Color{255,200,60,(unsigned char)(alpha*220)}
                                        : Color{255,140,140,(unsigned char)(alpha*220)};
            DrawText(bossName, sx+2, sy+2, ssz, {0,0,0,(unsigned char)(alpha*120)});
            DrawText(bossName, sx, sy, ssz, subCol);

            // Ligne déco sous le texte
            int lineY = sy + ssz + 6;
            int lineW = std::max(tw, sw2) + 40;
            DrawRectangle(sw/2-lineW/2, lineY, lineW, 3,
                          {mainColor.r, mainColor.g, mainColor.b, (unsigned char)(alpha*180)});
        }
    }

    void applyBrightness() {
        float b = _g->brightness;
        if (b < 1.f) {
            int sw=GetScreenWidth(), sh=GetScreenHeight();
            unsigned char dark=(unsigned char)((1.f-b)*220.f);
            DrawRectangle(0,0,sw,sh,{0,0,0,dark});
        }
    }

    void drawShield() {
        auto* pl=world.getComponent<PlayerComponent>(_player);
        auto* pos=world.getComponent<PositionComponent>(_player);
        if (!pl||!pos||!pl->hasShield||pl->shieldHp<=0.f) return;
        float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
        float ox=(GetScreenWidth()-1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;
        float cx=ox+(pos->x-_camX)*sc;
        float cy=oy+(pos->y-_camY)*sc;
        float ratio=_shieldMax>0?_shieldHp/_shieldMax:0;
        float pulse=1.f+0.09f*std::sin(GetTime()*4.f);
        float r=52.f*sc*pulse;
        DrawCircle((int)cx,(int)cy,(int)r,{0,60,200,22});
        Color sc2={(unsigned char)(60*(1-ratio)),(unsigned char)(100+80*ratio),255,210};
        DrawCircleLines((int)cx,(int)cy,(int)r,sc2);
        DrawCircleLines((int)cx,(int)cy,(int)(r+2),{sc2.r,sc2.g,sc2.b,90});
    }

    void drawEnemyHP() {
        float sc=std::min((float)GetScreenWidth()/1920.f,(float)GetScreenHeight()/1080.f);
        float ox=(GetScreenWidth()-1920.f*sc)/2.f;
        float oy=(GetScreenHeight()-1080.f*sc)/2.f;
        for (Entity e : world.getContainer<EnemyAIComponent>().getEntities()) {
            auto* pos=world.getComponent<PositionComponent>(e);
            auto* hp=world.getComponent<HealthComponent>(e);
            auto* ai=world.getComponent<EnemyAIComponent>(e);
            if (!pos||!hp||!ai) continue;
            float ratio=hp->maxHp>0?hp->hp/hp->maxHp:0;
            float bW=ai->isBoss?100.f:55.f, bH=ai->isBoss?12.f:7.f;
            float sx=ox+(pos->x-bW/2-_camX)*sc;
            float sy=oy+(pos->y-48-_camY)*sc;
            float sw=bW*sc, sh=bH*sc;
            if (sx<-bW||sx>GetScreenWidth()+bW) continue; // hors écran
            DrawRectangle((int)sx,(int)sy,(int)sw,(int)sh,DARKGRAY);
            Color hc=ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED;
            if(ai->isBoss) hc=ratio>0.5f?ORANGE:RED;
            DrawRectangle((int)sx,(int)sy,(int)(sw*ratio),(int)sh,hc);
            DrawRectangleLines((int)sx,(int)sy,(int)sw,(int)sh,WHITE);
        }
    }

    void drawCrosshair() {
        Vector2 m=GetMousePosition();
        int cx=(int)m.x, cy=(int)m.y;
        Color cc={255,255,255,210};
        DrawLine(cx-14,cy,cx-5,cy,cc); DrawLine(cx+5,cy,cx+14,cy,cc);
        DrawLine(cx,cy-14,cx,cy-5,cc); DrawLine(cx,cy+5,cx,cy+14,cc);
        DrawCircleLines(cx,cy,5,{255,255,255,150});
    }

    void drawHUD() {
        int sw=GetScreenWidth(), sh=GetScreenHeight();
        DrawRectangle(0,sh-58,sw,58,{0,0,0,235});
        DrawLine(0,sh-58,sw,sh-58,{40,40,65,255});
        int y=sh-58+10, sz=20;

        // HP bar
        float ratio=_maxHp>0?_hp/_maxHp:0;
        DrawRectangle(8,y,200,32,DARKGRAY);
        DrawRectangle(8,y,(int)(200*ratio),32,ratio>0.5f?GREEN:ratio>0.25f?YELLOW:RED);
        DrawRectangleLines(8,y,200,32,WHITE);
        DrawText(TextFormat("HP %.0f/%.0f",_hp,_maxHp),14,y+6,sz,WHITE);

        // Shield
        if (_hasShield) {
            float sr=_shieldMax>0?_shieldHp/_shieldMax:0;
            DrawRectangle(8,y-18,200,12,DARKGRAY);
            DrawRectangle(8,y-18,(int)(200*sr),12,{60,120,255,255});
            DrawRectangleLines(8,y-18,200,12,{100,180,255,255});
            DrawText(TextFormat("SHIELD %.0f",_shieldHp),10,y-16,10,{150,200,255,255});
        }

        DrawLine(216,sh-58,216,sh,{40,40,65,255});
        // Arme
        int ax=224;
        DrawText(TextFormat("[%s]",_wname.c_str()),ax,y,sz,_reloading?ORANGE:YELLOW);
        int ww=MeasureText(TextFormat("[%s]",_wname.c_str()),sz);
        DrawText(TextFormat("%d/%d",_ammo,_maxAmmo),ax+ww+6,y,sz,WHITE);
        if (_reloading) DrawText("RECHARGEMENT...",ax,y-18,13,ORANGE);
        DrawText(TextFormat("2:[%s]",_w2name.c_str()),ax,y+sz+1,13,GRAY);

        DrawLine(ax+210,sh-58,ax+210,sh,{40,40,65,255});
        // Vague
        int vx=ax+218;
        DrawText(TextFormat("Vague %d/6",_wave_n),vx,y,sz,_wstate==WaveState::BETWEEN_WAVES?ORANGE:WHITE);
        if (_wstate==WaveState::BETWEEN_WAVES)
            DrawText(TextFormat("Boss %.0fs",_betweenT),vx,y-18,13,ORANGE);

        DrawLine(vx+170,sh-58,vx+170,sh,{40,40,65,255});
        // Stats
        int stx=vx+178;
        DrawText(TextFormat("Mat:%d",_mats),stx,y,sz,GREEN);
        DrawText(TextFormat("Kills:%d",_kills),stx+90,y,sz,ORANGE);

        // Minimap (coin bas-droit)
        if (37 < (int)_g->_allSprites.size()) {
            int mmW=192, mmH=108;
            int mmX=sw-mmW-8, mmY=sh-58-mmH-8;
            DrawRectangle(mmX-2,mmY-2,mmW+4,mmH+4,{0,0,0,180});
            DrawTexturePro(_g->_allSprites[37],{0,0,480,270},{(float)mmX,(float)mmY,(float)mmW,(float)mmH},{0,0},0.f,WHITE);
            // Position joueur sur la minimap
            auto* pos=world.getComponent<PositionComponent>(_player);
            if (pos) {
                float px=mmX+pos->x/MAP_WORLD_W*mmW;
                float py=mmY+pos->y/MAP_WORLD_H*mmH;
                DrawCircle((int)px,(int)py,4,{0,220,255,255});
                DrawCircleLines((int)px,(int)py,4,WHITE);
            }
            DrawRectangleLines(mmX,mmY,mmW,mmH,{80,80,100,255});
        }

        // Volume
        DrawText(TextFormat("Vol:%d%%",(int)(_g->masterVolume*100)),
                 sw-90,sh-58+10,13,{80,80,80,255});
        // Controls hint
        const char* hint="[R]Reload [E]Pickup [Scroll/Clic D]Swap [Echap]Menu";
        int fsz=12;
        DrawText(hint,sw/2-MeasureText(hint,fsz)/2,sh-fsz-4,fsz,{60,60,70,255});
    }
};
