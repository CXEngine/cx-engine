#pragma once

#include <cx-engine/core/app.hpp>

#include <cx-engine/core/obj/object.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace cx {

class Weapon;
class Entity;
class Actor;

struct BulletHitInfo {
    Entity& target;
    sf::Vector2f pos;
};

class Bullet: public Object {
protected:
    Weapon& weapon;
    App& app;

public:
    Bullet(Weapon& weapon, App& app) : weapon(weapon), app(app) {}

    virtual void onHit(const BulletHitInfo& hit) noexcept {}
};

}
