#pragma once

#include "cx-engine/core/app.hpp"
#include <cx-engine/core/world/world.hpp>
#include <cx-engine/utils/hybrid-ptr.hpp>

#include <cx-engine/core/obj/sprite-object.hpp>

namespace cx {

class Actor;
class Weapon;

class WeaponBehavior {
public:
    Actor& owner;
    Weapon& weapon;
    World& world;
    App& app;

    WeaponBehavior(Actor& owner, Weapon& weapon, World& world, App& app)
        : owner(owner), weapon(weapon), world(world), app(app) {}
    virtual ~WeaponBehavior() = default;

    virtual void pressShoot() {};
    virtual void releaseShoot() {};
    virtual void pressReload() {};
    virtual void releaseReload() {};
};

class Weapon: public SpriteObject {
protected:
    HybridPtr<WeaponBehavior> behavior;

public:
    Weapon(
        const sf::Texture& texture, const sf::IntRect& rect,
        HybridPtr<WeaponBehavior> behavior = nullptr
    ) : SpriteObject(texture, rect)
      , behavior(std::move(behavior))
    {}

    void setBehavior(HybridPtr<WeaponBehavior> beh) {
        behavior = std::move(beh);
    }
    WeaponBehavior* getBehavior() {
        return behavior.get();
    }

    Actor& getOwner() {
        return behavior->owner;
    }

    void pressShoot() { if (behavior != nullptr) behavior->pressShoot(); }
    void releaseShoot() { if (behavior != nullptr) behavior->releaseShoot(); }

    void pressReload() { if (behavior != nullptr) behavior->pressReload(); }
    void releaseReload() { if (behavior != nullptr) behavior->releaseReload(); }
};

}
