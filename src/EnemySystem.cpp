#include "../include/Engine.hpp"
#include <cmath>
#include <random>

static std::mt19937 rng(std::random_device{}());

// Spawn un projectile ennemi
static void spawnEnemyBullet(World& world, float ox, float oy,
                              float dx, float dy, float speed, float dmg)
{
    Entity e = world.createEntity();

    PositionComponent pos; pos.x = ox; pos.y = oy;
    world.addComponent(e, pos);

    float len = sqrtf(dx*dx + dy*dy);
    if (len > 0.f) { dx /= len; dy /= len; }

    RigidbodyComponent rb; rb.vx = dx * speed; rb.vy = dy * speed;
    world.addComponent(e, rb);

    BulletComponent b;
    b.vx = rb.vx; b.vy = rb.vy;
    b.damage = dmg; b.speed = speed;
    b.fromPlayer = false; b.lifetime = 4.f;
    world.addComponent(e, b);

    SpriteComponent spr;
    spr.texturePath = 4; // << ASSET: assets/textures/bullet_enemy.png
    spr.width = 8.f; spr.height = 8.f;
    spr.tag = EntityTag::BULLET_EN;
    spr.layer = 3;
    world.addComponent(e, spr);

    BoxColliderComponent col;
    col.isCircle = true; col.radius = 4.f;
    col.TagsCollided = { (int)EntityTag::PLAYER };
    world.addComponent(e, col);
}

void EnemySystem::spawnEnemy(float x, float y, int wave, bool isBoss)
{
    if (!world) return;
    Entity e = world->createEntity();

    PositionComponent pos; pos.x = x; pos.y = y;
    world->addComponent(e, pos);

    // Stats scalées avec la vague
    float hpScale    = 1.f + (wave - 1) * 0.3f;
    float speedScale = 1.f + (wave - 1) * 0.05f;

    HealthComponent hp;
    hp.maxHp = hp.hp = isBoss ? 500.f * hpScale : 80.f * hpScale;
    world->addComponent(e, hp);

    RigidbodyComponent rb;
    world->addComponent(e, rb);

    SpriteComponent spr;
    spr.texturePath = isBoss ? 2 : 1; // << ASSET: enemy.png ou boss.png
    spr.width  = isBoss ? 64.f : 32.f;
    spr.height = isBoss ? 64.f : 32.f;
    spr.tag    = isBoss ? EntityTag::BOSS : EntityTag::ENEMY;
    spr.layer  = 2;
    world->addComponent(e, spr);

    BoxColliderComponent col;
    col.isCircle = true;
    col.radius   = isBoss ? 32.f : 16.f;
    col.TagsCollided = { (int)EntityTag::BULLET_PL, (int)EntityTag::PLAYER };
    world->addComponent(e, col);

    EnemyAIComponent ai;
    ai.isenemy    = true;
    ai.speed      = (isBoss ? 60.f : 100.f) * speedScale;
    ai.fireRate   = isBoss ? 0.8f : 2.0f;
    ai.bulletSpeed= isBoss ? 280.f : 180.f;
    ai.burstCount = isBoss ? 3 : 1;
    ai.spreadAngle= isBoss ? 0.25f : 0.f;
    ai.bossPhase  = isBoss ? 1 : 0;
    ai.behavior   = isBoss ? 2 : 1;
    world->addComponent(e, ai);
}

void EnemySystem::update(float dt)
{
    if (!world) return;

    auto& enemyAIs  = world->getContainer<EnemyAIComponent>();
    auto& positions = world->getContainer<PositionComponent>();
    auto& rigids    = world->getContainer<RigidbodyComponent>();
    auto& healths   = world->getContainer<HealthComponent>();
    auto& sprites   = world->getContainer<SpriteComponent>();

    // Trouver le joueur
    Entity playerEnt{0};
    float  playerX = 960.f, playerY = 540.f;
    for (Entity e : sprites.getEntities()) {
        auto* spr = sprites.get(e);
        if (spr && spr->tag == EntityTag::PLAYER) {
            auto* pos = positions.get(e);
            if (pos) { playerX = pos->x; playerY = pos->y; playerEnt = e; }
            break;
        }
    }

    std::vector<Entity> toRemove;

    for (Entity e : enemyAIs.getEntities()) {
        auto* ai  = enemyAIs.get(e);
        auto* pos = positions.get(e);
        auto* rb  = rigids.get(e);
        auto* hp  = healths.get(e);
        if (!ai || !pos || !rb) continue;

        // Mort
        if (hp && hp->isDead) {
            // Spawn XP orb
            Entity xp = world->createEntity();
            PositionComponent xpos; xpos.x = pos->x; xpos.y = pos->y;
            world->addComponent(xp, xpos);
            SpriteComponent xspr;
            xspr.texturePath = 9; // << ASSET: assets/textures/xp_orb.png
            xspr.width = 12.f; xspr.height = 12.f;
            xspr.tag = EntityTag::XP_ORB; xspr.layer = 1;
            world->addComponent(xp, xspr);
            BoxColliderComponent xcol;
            xcol.isCircle = true; xcol.radius = 12.f;
            xcol.TagsCollided = { (int)EntityTag::PLAYER };
            world->addComponent(xp, xcol);

            toRemove.push_back(e);
            continue;
        }

        // Mise à jour boss phase 2 (hp < 50%)
        if (ai->bossPhase > 0 && hp) {
            if (hp->hp < hp->maxHp * 0.5f && ai->bossPhase == 1) {
                ai->bossPhase  = 2;
                ai->fireRate   = 0.4f;
                ai->burstCount = 5;
                ai->speed     *= 1.3f;
                ai->spreadAngle = 0.4f;
            }
        }

        // Mouvement vers le joueur
        float dx = playerX - pos->x, dy = playerY - pos->y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > 1.f) {
            float ndx = dx/dist, ndy = dy/dist;
            rb->vx = ndx * ai->speed;
            rb->vy = ndy * ai->speed;
        } else {
            rb->vx = rb->vy = 0.f;
        }

        // Rotation vers joueur
        auto* spr = sprites.get(e);
        if (spr) spr->rotation = atan2f(dy, dx) * RAD2DEG;

        // Tir
        ai->lastFireTime += dt;
        if (ai->lastFireTime >= ai->fireRate && dist < ai->attackRange) {
            ai->lastFireTime = 0.f;

            if (ai->burstCount <= 1) {
                spawnEnemyBullet(*world, pos->x, pos->y, dx, dy,
                                 ai->bulletSpeed, 10.f);
            } else {
                // Burst en éventail (boss)
                float baseAngle = atan2f(dy, dx);
                float step = ai->spreadAngle;
                float start = baseAngle - step * (ai->burstCount - 1) / 2.f;
                for (int i = 0; i < ai->burstCount; ++i) {
                    float a = start + step * i;
                    spawnEnemyBullet(*world, pos->x, pos->y,
                                     cosf(a), sinf(a),
                                     ai->bulletSpeed, 10.f);
                }
            }
        }
    }

    for (Entity e : toRemove) {
        world->removeEntity(e);
        // Décrémenter compteur vague
        auto& waveComps = world->getContainer<WaveComponent>();
        for (Entity w : waveComps.getEntities()) {
            if (auto* wc = waveComps.get(w)) {
                if (wc->enemiesAlive > 0) wc->enemiesAlive--;
            }
        }
    }
}
