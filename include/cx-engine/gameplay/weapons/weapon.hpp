#pragma once

#include <cx-engine/core/app.hpp>
#include <cx-engine/core/world/world.hpp>
#include <cx-engine/utils/hybrid-ptr.hpp>

#include <cx-engine/core/entity/entity.hpp>

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

class Weapon: public Entity {
protected:
    HybridPtr<WeaponBehavior> behavior;

public:
    template <typename... TArgs>
    Weapon(
        HybridPtr<WeaponBehavior> behavior = nullptr,
        TArgs&&... args
    ) : Entity(std::forward<TArgs>(args)...)
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
