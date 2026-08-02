#pragma once

#include <cx-engine/core/app.hpp>
#include <cx-engine/gameplay/weapons/weapon.hpp>
#include <cx-engine/gameplay/weapons/bullet.hpp>

#include <cx-engine/core/entity/components/health.hpp>
#include <cx-engine/core/entity/components/sprite.hpp>

namespace cx {

class SimpleBullet: public Bullet {
public:
    SimpleBullet(Weapon& weapon, App& app) : Bullet(weapon, app) {
        setPosition(weapon.getPosition());
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(sprite().getSprite(), states);
    }

    void update(float dt) override {
        setPosition(getPosition() + velocity * dt);
    }

    void onHit(const BulletHitInfo& hit) noexcept override {
        if ((Actor*) &hit.target == &weapon.getOwner()) {
            return;
        }
        if (auto* health = hit.target.getComponent<HealthComponent>()) {
            health->takeDamage(Damage(20, DamageType::Physical));
        }
        isAlive = false;
    }
};

}
