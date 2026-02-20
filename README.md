# ZIMEX 🎮
**Brotato-style top-down survival shooter — Epitech BTTF CPP**

## Compilation

```bash
# Installer Raylib (Linux)
sudo apt install libraylib-dev   # ou compiler depuis source

# Build
make

# Lancer
./zimex
# ou
make run
```

## Contrôles (documentés comme requis par le PDF)

| Action | Touche |
|--------|--------|
| Mouvement | WASD / Flèches |
| Viser | Souris (le joueur regarde vers le curseur) |
| Tirer | Clic gauche |
| Recharger | R |
| Changer d'arme | Clic droit ou Scroll |
| Ramasser arme | Automatique au contact |

## Armes (mécaniques distinctes — requis PDF)

| Arme | Mécanique | Ammo |
|------|-----------|------|
| **Pistol** | Hitscan instantané (rayon, pas de projectile physique) | 12 |
| **Shotgun** | Spray de 6 projectiles en cône aléatoire | 6 |
| **Sniper** | Projectile perforant traversant 3 ennemis | 5 |
| **Rocket** | Projectile lent + explosion AoE radius 80px | 3 |

## Assets à placer dans `assets/`

### Textures (assets/textures/)
| Fichier | Index | Requis | Source suggérée |
|---------|-------|--------|-----------------|
| `player.png` | 0 | ✅ | Sprite 32x32 (perso Brotato) |
| `enemy.png` | 1 | ✅ | Sprite 32x32 ennemi |
| `boss.png` | 2 | ✅ | Sprite 64x64 boss |
| `bullet_player.png` | 3 | ✅ | Sprite 8x8 balle |
| `bullet_enemy.png` | 4 | ✅ | Sprite 8x8 balle rouge |
| `pickup_pistol.png` | 5 | ⚡ | Icône arme 24x24 |
| `pickup_shotgun.png` | 6 | ⚡ | Icône arme 24x24 |
| `pickup_sniper.png` | 7 | ✅ | Icône arme 24x24 |
| `pickup_rocket.png` | 8 | ✅ | Icône arme 24x24 |
| `xp_orb.png` | 9 | ✅ | Orbe vert 12x12 |
| `background.png` | 10 | ✅ | Fond 1920x1080 |
| `logo.png` | 11 | ⚡ | Logo optionnel |

> ⚡ = optionnel (placeholder rose utilisé si absent)
> ✅ = requis pour un rendu correct (placeholder rose si absent)

### Sons (assets/sounds/)
| Fichier | Index | Format |
|---------|-------|--------|
| `shoot_pistol.wav` | 0 | WAV |
| `shoot_shotgun.wav` | 1 | WAV |
| `shoot_rocket.wav` | 2 | WAV |
| `enemy_die.wav` | 3 | WAV |
| `pickup.wav` | 4 | WAV |
| `reload.wav` | 5 | WAV |
| `music_menu.ogg` | 6 | OGG |
| `music_game.ogg` | 7 | OGG |

> Si un son est absent, un son silencieux est utilisé automatiquement.

### Sources pour les assets gratuits
- **Itch.io (Brotato Demo)**: https://thomasgvd.itch.io/brotato-legacy-demo — extraire les sprites
- **Kenney.nl**: https://kenney.nl/assets — assets libres de droits top-down
- **OpenGameArt**: https://opengameart.org — sprites, sons gratuits
- **Freesound.org**: https://freesound.org — sons libres

## Architecture ECS

```
World
├── Entity (ID unique)
└── ComponentContainer<T>
    ├── PositionComponent   - coordonnées x,y
    ├── RigidbodyComponent  - vélocité vx,vy
    ├── HealthComponent     - hp/maxHp
    ├── SpriteComponent     - texture + tag EntityTag
    ├── BoxColliderComponent- AABB ou cercle
    ├── InputComponent      - marqueur joueur
    ├── PlayerComponent     - stats Brotato (speed, materials...)
    ├── WeaponComponent     - IWeapon current + second
    ├── BulletComponent     - vélocité + lifetime
    ├── EnemyAIComponent    - comportement + bossPhase
    ├── WaveComponent       - état vague (FIGHTING/CLEAR/GAMEOVER)
    └── AnimationComponent  - frame courante

Systems (update chaque frame):
  InputSystem      → WASD, souris, tir, reload, swap
  CollisionSystem  → AABB + cercle, par EntityTag
  PhysicSystem     → intégration vélocité → position
  BulletSystem     → lifetime, pierce, explosion AoE
  EnemySystem      → IA, spawn ennemis, boss phases
  WaveSystem       → gestion des 20 vagues Brotato
  WeaponSystem     → update cooldowns/reload IWeapon
  HealthSystem     → mort entités
  AnimationSystem  → frames spritesheet
  SoundSystem      → triggers sons
  RenderSystem     → rendu sprites/textes, virtual 1920x1080
```

## Validation PDF (checklist)

- [x] **Player Controls**: WASD, souris, clic gauche tir, scroll/clic droit swap, R reload
- [x] **Weapon abstraction OOP**: `IWeapon` interface avec `fire()` virtuel pur
- [x] **2+ armes mécaniquement distinctes**: Pistol (hitscan), Shotgun (spray), Sniper (pierce), Rocket (AoE)
- [x] **Ammo system**: currentAmmo/maxAmmo/reload par arme
- [x] **Weapon pickup**: armes spawned au sol, ramassage automatique au contact
- [x] **Weapon swap**: scroll ou clic droit
- [x] **Enemy system**: 1 type scalable + boss (x3 HP, spread pattern, phase 2)
- [x] **Collision**: joueur/mur (arène), projectile/ennemi, joueur/pickup
- [x] **Main Menu**: Start, Settings, Quit
- [x] **Settings**: Volume (+/-), Fullscreen toggle
- [x] **HUD**: HP bar, ammo, nom arme, vague, matériaux
- [x] **Raylib avec wrappers C++**: tous les composants/systèmes sont des classes C++
