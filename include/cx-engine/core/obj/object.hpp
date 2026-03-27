#pragma once

#include <cx-engine/systems/collision.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Window/Event.hpp>

#include <cmath>

namespace cx {

class Object: public sf::Transformable, public sf::Drawable {
protected:
    sf::Vector2f velocity;
    sf::Vector2f collisionBoxSize;

    virtual void onPositionUpdate() {}

public:
    bool isAlive = true;

public:
    Object();
    virtual ~Object() = default;

    virtual void handle(const sf::Event& event) {}
    virtual void update(float dt) {}
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override {}

    const sf::Vector2f& getVelocity() const;
    void setVelocity(const sf::Vector2f& vel);

    void setCollisionBoxSize(const sf::Vector2f& size);
    sf::Vector2f getCollisionBoxSize() const;
    virtual cx::Polygon getCollisionPolygon() const;

    sf::Vector2f forward() const {
        sf::Angle angle = getRotation();
        float rad = angle.asRadians();
        return { std::cos(rad), std::sin(rad) };
    }

    sf::Vector2f right() const {
        auto f = forward();
        return { f.y, -f.x };
    }

    void moveAndCollide(const sf::Vector2f& desiredMovement, Slice<cx::Polygon> obstacles);
};

} // namespace cx
