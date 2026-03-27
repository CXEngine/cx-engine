#pragma once

#include "cx-engine/core/app.hpp"
#include "cx-engine/gameplay/weapons/weapon.hpp"
#include "cx-engine/systems/global-texture-atlas.hpp"
#include <cx-engine/gameplay/weapons/bullet.hpp>

#include <cx-engine/core/entity/components/health.hpp>

namespace cx {

class SimpleBullet: public Bullet {
protected:
    static inline Optional<GlobalSpriteID> spriteId;
    static inline sf::Texture bulletTexture;

public:
    SimpleBullet(Weapon& weapon, App& app) : Bullet(weapon, app) {
        if (!spriteId.has_value()) {
            bulletTexture = app.assets.loadTexture("images/bullets/bullet.png");
            spriteId = app.globalSpriteAtlas.addSprite(bulletTexture);
        }
        setPosition(weapon.getPosition());
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        const sf::Sprite& sprite = app.globalSpriteAtlas.getSprite(*spriteId);
        states.transform *= getTransform();
        target.draw(sprite, states);
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
