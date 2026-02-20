#pragma once
#include "Engine.hpp"
#include <memory>

// Déclaration forward de createWeapon
std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world);

// ================================================================
// MENU SCENE
// ================================================================
class MenuScene : public Scene {
private:
    SceneManager*    _sm;
    GlobalComponent* _global;
    // Entités UI
    Entity _btnStart{0}, _btnSettings{0}, _btnQuit{0}, _title{0};
public:
    MenuScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Menu"), _sm(&sm), _global(&g) {}

    void onEnter() override {
        // Fond
        Entity bg = world.createEntity();
        PositionComponent bpos; bpos.x = 960; bpos.y = 540;
        world.addComponent(bg, bpos);
        SpriteComponent bspr;
        bspr.texturePath = 10; // << ASSET: assets/textures/background.png
        bspr.tag = EntityTag::UI; bspr.layer = 0;
        world.addComponent(bg, bspr);

        // Titre
        _title = world.createEntity();
        PositionComponent tpos; tpos.x = 760; tpos.y = 180;
        world.addComponent(_title, tpos);
        TextComponent ttxt;
        ttxt.text = "ZIMEX"; ttxt.size = 80;
        ttxt.font_color = {255, 220, 0, 255}; ttxt.layer = 5;
        world.addComponent(_title, ttxt);

        // Sous-titre
        Entity sub = world.createEntity();
        PositionComponent spos; spos.x = 760; spos.y = 280;
        world.addComponent(sub, spos);
        TextComponent stxt;
        stxt.text = "Brotato-style Survival"; stxt.size = 28;
        stxt.font_color = WHITE; stxt.layer = 5;
        world.addComponent(sub, stxt);

        // Bouton Start
        _btnStart = world.createEntity();
        PositionComponent p1; p1.x = 810; p1.y = 420;
        world.addComponent(_btnStart, p1);
        TextComponent t1; t1.text = "> Start Game"; t1.size = 40;
        t1.font_color = {100, 255, 100, 255}; t1.layer = 5;
        world.addComponent(_btnStart, t1);

        // Bouton Settings
        _btnSettings = world.createEntity();
        PositionComponent p2; p2.x = 810; p2.y = 500;
        world.addComponent(_btnSettings, p2);
        TextComponent t2; t2.text = "> Settings"; t2.size = 40;
        t2.font_color = WHITE; t2.layer = 5;
        world.addComponent(_btnSettings, t2);

        // Bouton Quit
        _btnQuit = world.createEntity();
        PositionComponent p3; p3.x = 810; p3.y = 580;
        world.addComponent(_btnQuit, p3);
        TextComponent t3; t3.text = "> Quit"; t3.size = 40;
        t3.font_color = {255, 80, 80, 255}; t3.layer = 5;
        world.addComponent(_btnQuit, t3);

        // Instructions
        Entity inst = world.createEntity();
        PositionComponent ipos; ipos.x = 760; ipos.y = 900;
        world.addComponent(inst, ipos);
        TextComponent itxt;
        itxt.text = "WASD: Move  |  Mouse: Aim  |  LClick: Shoot  |  R: Reload  |  RClick/Scroll: Swap Weapon";
        itxt.size = 20; itxt.font_color = GRAY; itxt.layer = 5;
        world.addComponent(inst, itxt);
    }

    void onExit() override { world.clearWorld(); }

    void update(float dt) override {
        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
        Vector2 m = GetMousePosition();

        // Hit test simple sur les boutons (zone approximative)
        auto hitBtn = [&](float bx, float by, float w, float h) {
            return m.x >= bx && m.x <= bx+w && m.y >= by && m.y <= by+h;
        };

        if (hitBtn(810, 420, 300, 50)) _sm->changeScene(SceneType::GAME);
        if (hitBtn(810, 500, 300, 50)) _sm->changeScene(SceneType::SETTINGS);
        if (hitBtn(810, 580, 200, 50)) CloseWindow();
    }

    void render(float dt) override {}
};

// ================================================================
// SETTINGS SCENE
// ================================================================
class SettingsScene : public Scene {
private:
    SceneManager*    _sm;
    GlobalComponent* _global;
public:
    SettingsScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Settings"), _sm(&sm), _global(&g) {}

    void onEnter() override {
        Entity t = world.createEntity();
        PositionComponent tp; tp.x = 760; tp.y = 200;
        world.addComponent(t, tp);
        TextComponent tt; tt.text = "SETTINGS"; tt.size = 60;
        tt.font_color = WHITE; tt.layer = 5;
        world.addComponent(t, tt);

        // Volume label
        Entity vl = world.createEntity();
        PositionComponent vp; vp.x = 760; vp.y = 380;
        world.addComponent(vl, vp);
        TextComponent vt; vt.text = "Volume:"; vt.size = 36;
        vt.font_color = WHITE; vt.layer = 5;
        world.addComponent(vl, vt);

        // Fullscreen label
        Entity fl = world.createEntity();
        PositionComponent fp; fp.x = 760; fp.y = 480;
        world.addComponent(fl, fp);
        TextComponent ft;
        ft.text = _global->isFullscreen ? "Fullscreen: ON" : "Fullscreen: OFF";
        ft.size = 36; ft.font_color = WHITE; ft.layer = 5;
        world.addComponent(fl, ft);

        // Back
        Entity bk = world.createEntity();
        PositionComponent bp; bp.x = 760; bp.y = 600;
        world.addComponent(bk, bp);
        TextComponent bt; bt.text = "< Back to Menu"; bt.size = 36;
        bt.font_color = {100, 200, 255, 255}; bt.layer = 5;
        world.addComponent(bk, bt);
    }

    void onExit() override { world.clearWorld(); }

    void update(float dt) override {
        Vector2 m = GetMousePosition();

        // Volume: +/- avec les touches
        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
            _global->masterVolume = std::min(1.f, _global->masterVolume + 0.1f);
            SetMasterVolume(_global->masterVolume);
        }
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            _global->masterVolume = std::max(0.f, _global->masterVolume - 0.1f);
            SetMasterVolume(_global->masterVolume);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Toggle fullscreen
            if (m.x >= 760 && m.x <= 1060 && m.y >= 480 && m.y <= 530) {
                _global->isFullscreen = !_global->isFullscreen;
                ToggleFullscreen();
            }
            // Back
            if (m.x >= 760 && m.x <= 1060 && m.y >= 600 && m.y <= 650)
                _sm->changeScene(SceneType::MENU);
        }
    }

    void render(float dt) override {
        // Slider volume (dessiné directement)
        int vx = 920, vy = 380, vw = 300, vh = 36;
        DrawRectangle(vx, vy, vw, vh, DARKGRAY);
        DrawRectangle(vx, vy, (int)(_global->masterVolume * vw), vh, GREEN);
        DrawText(TextFormat("%.0f%%", _global->masterVolume * 100), vx + vw + 10, vy, 36, WHITE);
        DrawText("(+/-) to adjust", vx, vy + 45, 22, GRAY);
    }
};

// ================================================================
// GAME OVER SCENE
// ================================================================
class GameOverScene : public Scene {
private:
    SceneManager* _sm;
public:
    GameOverScene(SceneManager& sm) : Scene("GameOver"), _sm(&sm) {}

    void onEnter() override {
        Entity t = world.createEntity();
        PositionComponent tp; tp.x = 660; tp.y = 380;
        world.addComponent(t, tp);
        TextComponent tt; tt.text = "GAME OVER"; tt.size = 90;
        tt.font_color = RED; tt.layer = 5;
        world.addComponent(t, tt);

        Entity b = world.createEntity();
        PositionComponent bp; bp.x = 810; bp.y = 560;
        world.addComponent(b, bp);
        TextComponent bt; bt.text = "Press ENTER to return to menu";
        bt.size = 32; bt.font_color = WHITE; bt.layer = 5;
        world.addComponent(b, bt);
    }

    void onExit() override { world.clearWorld(); }

    void update(float dt) override {
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            _sm->changeScene(SceneType::MENU);
    }

    void render(float dt) override {}
};

// ================================================================
// VICTORY SCENE
// ================================================================
class VictoryScene : public Scene {
private:
    SceneManager* _sm;
public:
    VictoryScene(SceneManager& sm) : Scene("Victory"), _sm(&sm) {}

    void onEnter() override {
        Entity t = world.createEntity();
        PositionComponent tp; tp.x = 620; tp.y = 350;
        world.addComponent(t, tp);
        TextComponent tt; tt.text = "YOU SURVIVED!"; tt.size = 80;
        tt.font_color = {255, 220, 0, 255}; tt.layer = 5;
        world.addComponent(t, tt);

        Entity s = world.createEntity();
        PositionComponent sp; sp.x = 760; sp.y = 460;
        world.addComponent(s, sp);
        TextComponent st; st.text = "20 Waves Completed!"; st.size = 40;
        st.font_color = WHITE; st.layer = 5;
        world.addComponent(s, st);

        Entity b = world.createEntity();
        PositionComponent bp; bp.x = 810; bp.y = 580;
        world.addComponent(b, bp);
        TextComponent bt; bt.text = "Press ENTER to play again";
        bt.size = 32; bt.font_color = {100, 255, 100, 255}; bt.layer = 5;
        world.addComponent(b, bt);
    }

    void onExit() override { world.clearWorld(); }

    void update(float dt) override {
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            _sm->changeScene(SceneType::MENU);
    }

    void render(float dt) override {}
};

// ================================================================
// GAME SCENE - scène principale Brotato
// ================================================================
class GameScene : public Scene {
private:
    SceneManager*    _sm;
    GlobalComponent* _global;

    // Systems
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

    Entity _player{0};
    Entity _waveEntity{0};

    // HUD data
    float  _hudHp = 100.f, _hudMaxHp = 100.f;
    int    _hudAmmo = 0, _hudMaxAmmo = 0;
    std::string _hudWeaponName = "Pistol";
    int    _hudWave = 1;
    int    _hudMaterials = 0;

public:
    GameScene(SceneManager& sm, GlobalComponent& g)
        : Scene("Game"), _sm(&sm), _global(&g) {}

    void initSystems() {
        auto initSys = [&](System& s) { s.setWorld(world); };
        initSys(_render);   _render.setGlobal(*_global);    _render.initRenderTarget();
        initSys(_input);
        initSys(_collision);
        initSys(_physics);
        initSys(_health);
        initSys(_animation); _animation.setGlobal(*_global);
        initSys(_weapon);   _weapon.setGlobal(*_global);
        initSys(_enemy);    _enemy.setGlobal(*_global);     _enemy.setSceneManager(*_sm);
        initSys(_bullet);
        initSys(_wave);     _wave.setSceneManager(*_sm);    _wave.setGlobal(*_global);
        initSys(_sound);    _sound.setGlobal(*_global);
    }

    void spawnPlayer() {
        _player = world.createEntity();

        PositionComponent pos; pos.x = 960.f; pos.y = 540.f;
        world.addComponent(_player, pos);

        HealthComponent hp; hp.hp = hp.maxHp = 100.f;
        world.addComponent(_player, hp);

        RigidbodyComponent rb;
        world.addComponent(_player, rb);

        SpriteComponent spr;
        spr.texturePath = 0; // << ASSET: assets/textures/player.png
        spr.width = 32.f; spr.height = 32.f;
        spr.tag = EntityTag::PLAYER; spr.layer = 2;
        spr.offsetX = -16.f; spr.offsetY = -16.f;
        world.addComponent(_player, spr);

        BoxColliderComponent col;
        col.isCircle = true; col.radius = 14.f;
        col.TagsCollided = {
            (int)EntityTag::BULLET_EN,
            (int)EntityTag::PICKUP,
            (int)EntityTag::XP_ORB,
        };
        world.addComponent(_player, col);

        InputComponent inp;
        world.addComponent(_player, inp);

        PlayerComponent pl;
        world.addComponent(_player, pl);

        // Arme de départ: pistol
        WeaponComponent wc;
        wc.currentWeapon = createWeapon("pistol", &world);
        wc.secondWeapon  = createWeapon("shotgun", &world);
        world.addComponent(_player, wc);
    }

    void spawnWaveController() {
        _waveEntity = world.createEntity();
        PositionComponent wp; wp.x = 0; wp.y = 0;
        world.addComponent(_waveEntity, wp);

        WaveComponent wc;
        wc.currentWave = 1;
        wc.state       = WaveState::FIGHTING;
        wc.enemiesLeft = wc.baseEnemiesPerWave;
        wc.spawnRate   = 1.5f;
        wc.waveDuration= 30.f;
        world.addComponent(_waveEntity, wc);
    }

    void spawnPickups() {
        // Spawn 2 armes au sol au début
        auto spawnPickup = [&](float x, float y, const std::string& type, int texIdx) {
            Entity e = world.createEntity();
            PositionComponent pp; pp.x = x; pp.y = y;
            world.addComponent(e, pp);
            SpriteComponent ps;
            ps.texturePath = texIdx;
            ps.width = 24.f; ps.height = 24.f;
            ps.tag = EntityTag::PICKUP; ps.layer = 1;
            world.addComponent(e, ps);
            BoxColliderComponent pc;
            pc.isCircle = true; pc.radius = 16.f;
            pc.TagsCollided = { (int)EntityTag::PLAYER };
            world.addComponent(e, pc);
            WeaponComponent wc;
            wc.isPickup = true; wc.pickupWeaponType = type;
            world.addComponent(e, wc);
        };

        spawnPickup(700, 400, "sniper", 7);  // << ASSET: pickup_sniper.png
        spawnPickup(1200, 650, "rocket", 8); // << ASSET: pickup_rocket.png
    }

    void onEnter() override {
        initSystems();
        spawnPlayer();
        spawnWaveController();
        spawnPickups();
    }

    void onExit() override { world.clearWorld(); }

    void update(float dt) override {
        _input.update(dt);
        _collision.update(dt);
        _physics.update(dt);
        _bullet.update(dt);
        _enemy.update(dt);
        _wave.update(dt);
        _weapon.update(dt);
        _sound.update(dt);
        _animation.update(dt);

        // Gestion pickup arme
        handlePickups();
        // Gestion dégâts joueur
        handlePlayerDamage(dt);
        // Vérif game over
        checkGameOver();
        // Sync HUD
        syncHUD();
    }

    void handlePickups() {
        auto* col = world.getComponent<BoxColliderComponent>(_player);
        if (!col) return;

        std::vector<Entity> toRemove;
        for (int otherId : col->EntityCollided) {
            Entity other = static_cast<Entity>(otherId);
            auto* spr = world.getComponent<SpriteComponent>(other);
            if (!spr || spr->tag != EntityTag::PICKUP) continue;
            auto* pwc = world.getComponent<WeaponComponent>(other);
            if (!pwc || !pwc->isPickup) continue;

            // Donner l'arme au joueur
            auto* plwc = world.getComponent<WeaponComponent>(_player);
            if (plwc) {
                plwc->secondWeapon = createWeapon(pwc->pickupWeaponType, &world);
            }
            toRemove.push_back(other);
        }
        for (Entity e : toRemove) world.removeEntity(e);

        // Collecter XP orbs
        auto* pl = world.getComponent<PlayerComponent>(_player);
        for (int otherId : col->EntityCollided) {
            Entity other = static_cast<Entity>(otherId);
            auto* spr = world.getComponent<SpriteComponent>(other);
            if (!spr || spr->tag != EntityTag::XP_ORB) continue;
            if (pl) pl->materials++;
            world.removeEntity(other);
        }
    }

    void handlePlayerDamage(float dt) {
        auto* pl  = world.getComponent<PlayerComponent>(_player);
        auto* col = world.getComponent<BoxColliderComponent>(_player);
        auto* hp  = world.getComponent<HealthComponent>(_player);
        if (!pl || !col || !hp || pl->isInvincible) return;

        for (int otherId : col->EntityCollided) {
            Entity other = static_cast<Entity>(otherId);
            auto* spr = world.getComponent<SpriteComponent>(other);
            if (!spr) continue;

            bool dmg = false;
            if (spr->tag == EntityTag::BULLET_EN) {
                auto* b = world.getComponent<BulletComponent>(other);
                if (b) { hp->hp -= b->damage; dmg = true; }
                world.removeEntity(other);
            } else if (spr->tag == EntityTag::ENEMY || spr->tag == EntityTag::BOSS) {
                hp->hp -= 10.f * dt; // dégâts par contact
                dmg = true;
            }

            if (dmg) {
                pl->isInvincible = true;
                pl->invincTimer  = 0.8f;
                break;
            }
        }
    }

    void checkGameOver() {
        auto* hp = world.getComponent<HealthComponent>(_player);
        if (!hp) return;
        if (hp->hp <= 0.f) {
            auto* wc = world.getComponent<WaveComponent>(_waveEntity);
            if (wc) wc->state = WaveState::GAME_OVER;
        }
    }

    void syncHUD() {
        auto* hp  = world.getComponent<HealthComponent>(_player);
        auto* wc  = world.getComponent<WeaponComponent>(_player);
        auto* pl  = world.getComponent<PlayerComponent>(_player);
        auto* wvc = world.getComponent<WaveComponent>(_waveEntity);

        if (hp)  { _hudHp = hp->hp; _hudMaxHp = hp->maxHp; }
        if (wc)  { _hudAmmo = wc->ammo; _hudMaxAmmo = wc->maxAmmo; _hudWeaponName = wc->weaponName; }
        if (pl)  { _hudMaterials = pl->materials; }
        if (wvc) { _hudWave = wvc->currentWave; }
    }

    void render(float dt) override {
        _render.update(dt);
        drawHUD();
    }

    void drawHUD() {
        // Fond HUD en bas
        DrawRectangle(0, 1040, 1920, 40, { 0, 0, 0, 180 });

        // HP Bar
        DrawRectangle(20, 1047, 200, 26, DARKGRAY);
        float hpRatio = (_hudMaxHp > 0) ? _hudHp / _hudMaxHp : 0;
        Color hpColor = (hpRatio > 0.5f) ? GREEN : (hpRatio > 0.25f) ? YELLOW : RED;
        DrawRectangle(20, 1047, (int)(200 * hpRatio), 26, hpColor);
        DrawRectangleLines(20, 1047, 200, 26, WHITE);
        DrawText(TextFormat("HP %.0f/%.0f", _hudHp, _hudMaxHp), 25, 1050, 20, WHITE);

        // Arme + ammo
        DrawText(TextFormat("[%s]  %d / %d", _hudWeaponName.c_str(), _hudAmmo, _hudMaxAmmo),
                 260, 1050, 22, YELLOW);

        // Vague
        DrawText(TextFormat("Wave: %d / 20", _hudWave), 700, 1050, 22, WHITE);

        // Matériaux
        DrawText(TextFormat("Materials: %d", _hudMaterials), 950, 1050, 22, {100, 255, 100, 255});

        // Reloading indicator
        if (world.hasContainer<WeaponComponent>()) {
            auto* wc = world.getComponent<WeaponComponent>(_player);
            if (wc && wc->currentWeapon && wc->currentWeapon->isReloading)
                DrawText("RELOADING...", 1400, 1050, 22, ORANGE);
        }
    }
};
