#pragma once
#include <memory>
#include <string>
#include "Bullet.hpp"

// ================================================================
// INTERFACE WEAPON
// ================================================================
class IWeapon {
public:
    virtual ~IWeapon() = default;

    virtual bool        fire(float px, float py, float tx, float ty) = 0;
    virtual std::string getName()    const = 0;
    virtual std::string getDesc()    const = 0;
    virtual int         getCurrentAmmo() const { return currentAmmo; }
    virtual int         getMaxAmmo()     const { return maxAmmo; }
    virtual bool        canFire()        const {
        return currentAmmo > 0 && !isReloading && fireCooldown <= 0.f;
    }
    virtual void        reload() {
        if (!isReloading && currentAmmo < maxAmmo) {
            isReloading = true;
            reloadTimer = reloadTime;
        }
    }
    virtual void        refillAmmo() {  // pickup ammo
        currentAmmo = maxAmmo;
        isReloading = false;
        reloadTimer = 0.f;
    }
    virtual void update(float dt) {
        if (reloadTimer > 0.f) {
            reloadTimer -= dt;
            if (reloadTimer <= 0.f) {
                reloadTimer  = 0.f;
                isReloading  = false;
                currentAmmo  = maxAmmo;
            }
        }
        if (fireCooldown > 0.f) fireCooldown -= dt;
    }

    bool  isReloading   = false;
    float reloadTimer   = 0.f;

protected:
    int   currentAmmo   = 10;
    int   maxAmmo       = 10;
    float fireRate      = 0.3f;
    float fireCooldown  = 0.f;
    float reloadTime    = 1.5f;
};

// ================================================================
// COMPONENT
// ================================================================
struct WeaponSlot {
    std::shared_ptr<IWeapon> weapon;
    std::string type;  // "pistol", "shotgun", "sniper", "ak47", "bomb"
};

struct WeaponComponent {
    WeaponSlot  slots[2];          // slot 0 = actif, slot 1 = secondaire
    int         activeSlot = 0;
    bool        isPickup   = false;
    std::string pickupWeaponType = "";

    IWeapon* current() {
        return slots[activeSlot].weapon.get();
    }
    void swap() {
        activeSlot = 1 - activeSlot;
    }
    std::string weaponName()  const {
        auto& s = slots[activeSlot];
        return s.weapon ? s.weapon->getName() : "None";
    }
    int ammo()    const { auto& s=slots[activeSlot]; return s.weapon?s.weapon->getCurrentAmmo():0; }
    int maxAmmo() const { auto& s=slots[activeSlot]; return s.weapon?s.weapon->getMaxAmmo():0; }
    bool reloading() const { auto& s=slots[activeSlot]; return s.weapon&&s.weapon->isReloading; }
    float reloadPct()const {
        auto& s=slots[activeSlot];
        if(!s.weapon||!s.weapon->isReloading) return 0.f;
        return 1.f - s.weapon->reloadTimer / std::max(0.01f,s.weapon->reloadTimer);
    }
};
