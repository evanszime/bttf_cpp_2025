#include "../include/Engine.hpp"
#include <cmath>
#include <random>

// ================================================================
// WEAPON IMPLEMENTATIONS - 4 types mécaniquement distincts
// Pistol  : hitscan instantané (1 rayon, pas de projectile)
// Shotgun : projectiles multiples en cône (area spray)
// Sniper  : projectile unique perforant, longue portée
// Rocket  : projectile lent avec explosion en zone (AoE)
// ================================================================

// ---------------------------------------------------------------
// Helper: spawn un projectile dans le world
// ---------------------------------------------------------------
static Entity spawnBullet(World& world, float ox, float oy,
                          float dx, float dy, float speed,
                          float dmg, bool pierce = false, int pierceLeft = 0)
{
    Entity e = world.createEntity();

    PositionComponent pos; pos.x = ox; pos.y = oy;
    world.addComponent(e, pos);

    float len = sqrtf(dx*dx + dy*dy);
    if (len > 0.f) { dx /= len; dy /= len; }

    RigidbodyComponent rb; rb.vx = dx * speed; rb.vy = dy * speed;
    world.addComponent(e, rb);

    BulletComponent bullet;
    bullet.vx = rb.vx; bullet.vy = rb.vy;
    bullet.damage = dmg; bullet.speed = speed;
    bullet.fromPlayer = true;
    bullet.pierce = pierce; bullet.pierceLeft = pierceLeft;
    world.addComponent(e, bullet);

    SpriteComponent spr;
    spr.texturePath = 3; // << ASSET: assets/textures/bullet_player.png
    spr.width  = 8.f; spr.height = 8.f;
    spr.tag    = EntityTag::BULLET_PL;
    spr.layer  = 3;
    world.addComponent(e, spr);

    BoxColliderComponent col;
    col.isCircle = true; col.radius = 4.f;
    col.TagsCollided = { (int)EntityTag::ENEMY, (int)EntityTag::BOSS };
    world.addComponent(e, col);

    return e;
}

// ---------------------------------------------------------------
// PISTOL - hitscan (rayon instantané, pas de projectile physique)
// Différence clé: aucun BulletComponent, dégât immédiat via rayon
// ---------------------------------------------------------------
class Pistol : public IWeapon {
public:
    Pistol() {
        currentAmmo = 12; maxAmmo = 12;
        fireRate = 0.35f; reloadTime = 1.2f;
    }

    std::string getName() const override { return "Pistol"; }

    bool fire(float px, float py, float tx, float ty) override {
        if (!canFire() || !_world) return false;
        currentAmmo--;
        fireCooldown = fireRate;

        // Hitscan: chercher l'ennemi le plus proche sur la ligne de tir
        float dx = tx - px, dy = ty - py;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) return true;
        float ndx = dx/len, ndy = dy/len;

        auto& enemies  = _world->getContainer<SpriteComponent>();
        auto& healths  = _world->getContainer<HealthComponent>();
        auto& positions= _world->getContainer<PositionComponent>();

        Entity hit{0}; float bestDist = 800.f;

        for (Entity e : enemies.getEntities()) {
            auto* spr = enemies.get(e);
            if (!spr) continue;
            if (spr->tag != EntityTag::ENEMY && spr->tag != EntityTag::BOSS) continue;
            auto* pos = positions.get(e);
            if (!pos) continue;

            // Projection sur le rayon
            float ex = pos->x - px, ey = pos->y - py;
            float proj = ex*ndx + ey*ndy;
            if (proj < 0) continue;

            float perpX = ex - proj*ndx, perpY = ey - proj*ndy;
            float perp = sqrtf(perpX*perpX + perpY*perpY);

            if (perp < 20.f && proj < bestDist) {
                bestDist = proj; hit = e;
            }
        }

        if ((std::size_t)hit != 0) {
            if (auto* h = healths.get(hit))
                h->hp -= 15.f;
        }
        return true;
    }

    void setWorld(World* w) { _world = w; }
private:
    World* _world = nullptr;
};

// ---------------------------------------------------------------
// SHOTGUN - spray de projectiles en cône
// Différence clé: 6 projectiles simultanés, courte portée
// ---------------------------------------------------------------
class Shotgun : public IWeapon {
public:
    Shotgun() {
        currentAmmo = 6; maxAmmo = 6;
        fireRate = 0.8f; reloadTime = 2.0f;
    }

    std::string getName() const override { return "Shotgun"; }

    bool fire(float px, float py, float tx, float ty) override {
        if (!canFire() || !_world) return false;
        currentAmmo--;
        fireCooldown = fireRate;

        float baseAngle = atan2f(ty - py, tx - px);
        static std::mt19937 rng(42);
        std::uniform_real_distribution<float> spread(-0.35f, 0.35f); // ~20°

        // 6 projectiles en cône
        for (int i = 0; i < 6; ++i) {
            float angle = baseAngle + spread(rng);
            float dx = cosf(angle), dy = sinf(angle);
            spawnBullet(*_world, px, py, dx, dy, 320.f, 8.f);
        }
        return true;
    }

    void setWorld(World* w) { _world = w; }
private:
    World* _world = nullptr;
};

// ---------------------------------------------------------------
// SNIPER - projectile perforant, haute vélocité
// Différence clé: traverse plusieurs ennemis (pierce), 1 balle
// ---------------------------------------------------------------
class Sniper : public IWeapon {
public:
    Sniper() {
        currentAmmo = 5; maxAmmo = 5;
        fireRate = 1.2f; reloadTime = 2.5f;
    }

    std::string getName() const override { return "Sniper"; }

    bool fire(float px, float py, float tx, float ty) override {
        if (!canFire() || !_world) return false;
        currentAmmo--;
        fireCooldown = fireRate;
        float dx = tx - px, dy = ty - py;
        spawnBullet(*_world, px, py, dx, dy, 900.f, 40.f, true, 3);
        return true;
    }

    void setWorld(World* w) { _world = w; }
private:
    World* _world = nullptr;
};

// ---------------------------------------------------------------
// ROCKET LAUNCHER - projectile lent, explosion AoE
// Différence clé: dégâts en zone à l'impact (zone = 80px radius)
// ---------------------------------------------------------------
class RocketLauncher : public IWeapon {
public:
    RocketLauncher() {
        currentAmmo = 3; maxAmmo = 3;
        fireRate = 1.5f; reloadTime = 3.0f;
    }

    std::string getName() const override { return "Rocket"; }

    bool fire(float px, float py, float tx, float ty) override {
        if (!canFire() || !_world) return false;
        currentAmmo--;
        fireCooldown = fireRate;

        float dx = tx - px, dy = ty - py;
        Entity bullet = spawnBullet(*_world, px, py, dx, dy, 250.f, 0.f); // dégât géré à l'explosion

        // Marquer comme roquette dans le BulletComponent
        if (auto* b = _world->getComponent<BulletComponent>(bullet)) {
            b->damage  = 60.f;
            b->lifetime = 4.f;
            // On réutilise pierceLeft = -1 comme marqueur "rocket"
            b->pierceLeft = -99;
        }
        // Sprite roquette
        if (auto* spr = _world->getComponent<SpriteComponent>(bullet)) {
            spr->texturePath = 3; // même sprite pour l'instant << ASSET
            spr->width  = 16.f; spr->height = 8.f;
        }
        return true;
    }

    void setWorld(World* w) { _world = w; }
private:
    World* _world = nullptr;
};

// ================================================================
// WeaponSystem::update
// ================================================================

// Helper pour créer une arme par nom
static std::shared_ptr<IWeapon> makeWeapon(const std::string& type, World* world)
{
    if (type == "pistol") {
        auto w = std::make_shared<Pistol>(); w->setWorld(world); return w;
    } else if (type == "shotgun") {
        auto w = std::make_shared<Shotgun>(); w->setWorld(world); return w;
    } else if (type == "sniper") {
        auto w = std::make_shared<Sniper>(); w->setWorld(world); return w;
    } else if (type == "rocket") {
        auto w = std::make_shared<RocketLauncher>(); w->setWorld(world); return w;
    }
    return nullptr;
}

void WeaponSystem::update(float dt)
{
    if (!world) return;
    auto& weapons = world->getContainer<WeaponComponent>();

    for (Entity e : weapons.getEntities()) {
        auto* wc = weapons.get(e);
        if (!wc) continue;

        // Initialiser une arme pour les pickups
        if (wc->isPickup && !wc->currentWeapon && !wc->pickupWeaponType.empty()) {
            // Arme au sol: rien à update
            continue;
        }

        // Update des armes actives
        if (wc->currentWeapon) {
            wc->currentWeapon->update(dt);
            wc->ammo    = wc->currentWeapon->getCurrentAmmo();
            wc->maxAmmo = wc->currentWeapon->getMaxAmmo();
            wc->weaponName = wc->currentWeapon->getName();
        }
        if (wc->secondWeapon)
            wc->secondWeapon->update(dt);
    }
}

// Fonction utilitaire globale pour créer une arme (utilisée dans GameScene)
std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world) {
    return makeWeapon(type, world);
}
