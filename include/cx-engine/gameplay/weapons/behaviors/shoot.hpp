#pragma once

#include <cx-engine/gameplay/weapons/bullet.hpp>
#include <cx-engine/gameplay/weapons/weapon.hpp>

namespace cx {

struct ShootWeaponBehaviorConfig {
    float fireDelay = 10.0f;        // in miliseconds
    bool isAuto = false;            // is automatic

    float bulletSpeed = 2000.f;     // game scale units

    usize burstSize = 1;            // bullets in series
    float burstInterval = 0.1f;     // time between shots in series
    float burstCooldown = 0.0f;     // delay after full series

    usize bulletsPerShot = 1;       // shotgun-style

    float spreadAngleDeg = 0.0f;    // symmetric spread

    sf::Vector2f muzzleOffset = {0.f, 0.f};
};

template <typename TBullet>
requires std::is_base_of_v<Bullet, TBullet>
class ShootWeaponBehavior: public WeaponBehavior {
protected:

public:
    using Config = ShootWeaponBehaviorConfig;
    Config cfg;

public:
    ShootWeaponBehavior(Actor& owner, Weapon& weapon, World& world, App& app, Config cfg = {})
        : WeaponBehavior(owner, weapon, world, app)
        , cfg(cfg) {}

    void pressShoot() override {
        TBullet& bullet = world.objects.create<TBullet>(weapon, app);
        sf::Angle shootRot = weapon.getRotation() - sf::degrees(90);
        bullet.setRotation(shootRot);
        bullet.setScale({0.1f, 0.1f});
        bullet.setVelocity({
            (float) std::cos(shootRot.asRadians()) * cfg.bulletSpeed,
            (float) std::sin(shootRot.asRadians()) * cfg.bulletSpeed,
        });
    }

    void releaseShoot() override {}

    void pressReload() override {}
    void releaseReload() override {}
};

} // namespace cx
