#pragma once
#include <memory>
#include <string>

// ============================================================
// INTERFACE WEAPON - requis par le PDF (OOP abstraction)
// Chaque arme DOIT hériter de IWeapon et override fire()
// ============================================================
class IWeapon {
public:
    virtual ~IWeapon() = default;

    // Tenter de tirer - retourne true si le tir a eu lieu
    virtual bool fire(float playerX, float playerY, float targetX, float targetY) = 0;

    // Recharger
    virtual void reload() { currentAmmo = maxAmmo; isReloading = false; }

    // Getters obligatoires
    virtual std::string getName()    const = 0;
    virtual int getCurrentAmmo()     const { return currentAmmo; }
    virtual int getMaxAmmo()         const { return maxAmmo; }
    virtual bool canFire()           const { return currentAmmo > 0 && !isReloading && reloadTimer <= 0.f; }

    // Update (pour gérer cooldowns, reload timer)
    virtual void update(float dt) {
        if (reloadTimer > 0.f) {
            reloadTimer -= dt;
            if (reloadTimer <= 0.f) {
                reloadTimer = 0.f;
                isReloading = false;
                currentAmmo = maxAmmo;
            }
        }
        if (fireCooldown > 0.f)
            fireCooldown -= dt;
    }

    bool  isReloading  = false;

protected:
    int   currentAmmo  = 10;
    int   maxAmmo      = 10;
    float fireRate     = 0.3f;   // secondes entre deux tirs
    float fireCooldown = 0.f;
    float reloadTime   = 1.5f;   // secondes pour recharger
    float reloadTimer  = 0.f;
};

// ============================================================
// WEAPON COMPONENT - attaché à l'entité joueur
// ============================================================
struct WeaponComponent {
    std::shared_ptr<IWeapon> currentWeapon  = nullptr;
    std::shared_ptr<IWeapon> secondWeapon   = nullptr; // slot 2

    // Pour l'affichage HUD
    std::string weaponName   = "None";
    int         ammo         = 0;
    int         maxAmmo      = 0;

    // Pickup au sol
    bool        isPickup     = false;  // true = arme à ramasser
    std::string pickupWeaponType = ""; // "pistol" | "shotgun" | "sniper" | "rocket"
};
