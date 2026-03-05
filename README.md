# ZIMEX — Top-Down Survivor

**Engine:** C++17 + Raylib | **Style:** Brotato-inspired | **Build:** `make re && ./zimex`

---

## Player Controls

| Action | Binding |
|--------|---------|
| **Move Up** | `W` or `↑` |
| **Move Down** | `S` or `↓` |
| **Move Left** | `A` or `←` |
| **Move Right** | `D` or `→` |
| **Aim** | Mouse position (player rotates toward cursor) |
| **Shoot** | `Left Mouse Button` (hold for auto-fire) |
| **Weapon Swap** | `Right Mouse Button` or `Scroll Wheel` |
| **Reload** | `R` |
| **Pickup Weapon** | `E` (when near a weapon on the ground) |
| **Pause / Menu** | `Escape` |
| **Volume +/-** | `+` / `-` |
| **Brightness -/+** | `F5` / `F6` |
| **Fullscreen** | `F11` |

---

## Weapon System (OO Abstraction)

### Interface / Base Class

```cpp
// include/Components/Weapon.hpp
class IWeapon {
public:
    virtual bool        fire(float px, float py, float tx, float ty) = 0;
    virtual std::string getName()    const = 0;
    virtual std::string getDesc()    const = 0;
    virtual void        reload()           = 0;
    virtual void        update(float dt)   = 0;
    virtual int         getCurrentAmmo()   const;
    virtual int         getMaxAmmo()       const;
    virtual bool        canFire()          const;
    // ...
};
```

All weapon types inherit from `IWeapon`. No hardcoded weapon logic outside the derived classes.

### Derived Weapon Types

| Weapon | Mechanic | Ammo | Tactical Role |
|--------|----------|------|---------------|
| **Pistol** | Single hitscan projectile | 120 | Balanced, always available |
| **Shotgun** | 7 spread projectiles (AoE cone) | 60 | Close-range crowd control |
| **Sniper** | Pierce-through (x3 enemies), long range | 50 | Single-target, high damage |
| **Bomb Launcher** | AoE explosion (120px radius), splash damage | 30 | Area denial, splash damage |

**Mechanically distinct by design:**
- Pistol = single projectile, hitscan-style
- Shotgun = multi-projectile spread (fundamentally different hit pattern)
- Sniper = pierce effect (passes through multiple enemies)
- Bomb = AoE explosion on impact (completely different damage model)

### Weapon Factory

```cpp
// src/WeaponSystem.cpp
std::shared_ptr<IWeapon> createWeapon(const std::string& type, World* world) {
    if(type=="shotgun")  return std::make_shared<Shotgun>(world);
    if(type=="sniper")   return std::make_shared<Sniper>(world);
    if(type=="bomb")     return std::make_shared<BombLauncher>(world);
    return std::make_shared<Pistol>(world); // default
}
```

### Ammo System

- `currentAmmo` / `maxAmmo` tracked per weapon instance
- Cannot fire when `currentAmmo == 0`
- `R` key triggers reload → `isReloading = true` → refills after `reloadTime` seconds
- HUD displays current/max ammo + reload progress bar

### Weapon Pickup System

- Weapons spawn as entities on the map at game start
- Player walks near them → press `E` to equip into active slot
- Two weapon slots (slot 0 = active, slot 1 = secondary)
- Swap with `Right Click` or `Scroll Wheel`

---

## Enemy System

### Base Enemy (scalable behavior)

```cpp
// src/WaveSystem.cpp + src/EnemySystem.cpp
struct EnemyAIComponent {
    float speed, fireRate, bulletSpeed, damage;
    int   burstCount;
    float spreadAngle;
    bool  isBoss;
    BossType bossType;
    int   bossPhase;  // phase transitions at HP thresholds
};
```

- **Normal enemy:** single projectile, moderate speed, scales with wave number
- **Boss:** same `EnemyAIComponent`, but `isBoss=true` with different `BossType`

### Boss Variants (built on same enemy base)

| Wave | Boss | Special Mechanic |
|------|------|-----------------|
| 1 | Le Fusilleur Pourpre | Shotgun spread (7 purple bullets) |
| 2 | L'Oeil Blanc | Piercing white sniper shots |
| 3 | Le Bombardier | Yellow AoE bombs |
| 4 | La Fusée Rouge | Rapid red rockets |
| 5 | Le Maître des Ondes | Bombs + 16-directional shockwave + heals minions ×2 |
| 6 | BOSS FINAL | All bullet types, rage phases at 80/60/40/20% HP, upgrades minions |

### Phase Transitions (Boss Final)

```cpp
// Rage phase triggered every 20% HP loss
int phase = getFinalBossPhase(hp / maxHp);
// Increases: speed, fireRate, bulletSpeed, burstCount, spreadAngle
```

---

## Collision System

### Player-to-Wall (map rocks)

```cpp
// include/Scenes.hpp → resolveObstacles()
// AABB vs Circle push-out for each rock rectangle
void resolveObstacles() {
    for (auto& obs : MAP_OBSTACLES) {
        float nearX = max(obs.x, min(pos->x, obs.x + obs.width));
        float nearY = max(obs.y, min(pos->y, obs.y + obs.height));
        // push player out of rock
    }
}
```

Rock positions are defined in `include/MapObstacles.hpp` — auto-detected from the map image.

### Projectile-to-Wall

```cpp
// src/BulletSystem.cpp
for (const auto& obs : MAP_OBSTACLES) {
    if (bullet inside obs) → destroy bullet + spawn spark effect
}
```

### Projectile-to-Enemy

```cpp
// src/CollisionSystem.cpp → circCirc()
// Bullets (BULLET_PL) tagged to collide with ENEMY/BOSS tags
// BulletSystem.cpp reads col->EntityCollided → applies damage
```

### Player-to-Pickup (weapon acquisition)

```cpp
// include/Scenes.hpp → doPickupWeapon()
// Checks distance to all WeaponComponent entities with isPickup=true
// E key triggers equip → weapon moved to active slot
```

---

## Settings Persistence

Settings saved to `zimex_settings.cfg` (plain text, same folder as binary):

```
volume=0.80
brightness=1.00
fullscreen=0
```

- **Load:** on game startup (SplashScene)
- **Save:** when exiting the Settings menu (Escape or Back button)

```cpp
// include/GlobalComponent.hpp
void saveSettings(); // writes zimex_settings.cfg
void loadSettings(); // reads and applies zimex_settings.cfg
```

---

## Architecture

```
zimex/
├── include/
│   ├── Engine.hpp              # ECS core (World, Entity, Component containers)
│   ├── GlobalComponent.hpp     # Shared state: assets, audio, settings
│   ├── Scenes.hpp              # All scenes (Splash, Menu, Settings, Game, GameOver)
│   ├── MapObstacles.hpp        # Rock collision rectangles (auto-detected)
│   └── Components/
│       ├── Weapon.hpp          # IWeapon interface + WeaponComponent
│       ├── EnemyAI.hpp         # EnemyAIComponent + BossType enum
│       ├── Bullet.hpp          # BulletComponent + BulletEffect enum
│       └── ...
├── src/
│   ├── WeaponSystem.cpp        # IWeapon derived classes + factory
│   ├── EnemySystem.cpp         # Enemy AI, boss behaviors, shockwave
│   ├── WaveSystem.cpp          # Wave spawning, boss spawning
│   ├── BulletSystem.cpp        # Bullet movement + all collision resolution
│   ├── CollisionSystem.cpp     # AABB/Circle collision detection
│   ├── RenderSystem.cpp        # Camera, frustum culling, virtual 1920x1080
│   └── ...
└── assets/
    ├── textures/               # 38 PNG sprites (pixel art)
    └── sounds/                 # 14 OGG audio files
```

---

## Build

```bash
# Linux (requires libraylib-dev)
sudo apt install libraylib-dev
make re
./zimex

# Dependencies: Raylib 4.x, C++17
```
