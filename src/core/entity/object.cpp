#include <cx-engine/core/obj/object.hpp>
#include <cx-engine/core/math.hpp>

#include <cmath>

namespace cx {

Object::Object()
    : velocity({0.f, 0.f})
    , collisionBoxSize({0.f, 0.f})
{}

const sf::Vector2f& Object::getVelocity() const {
    return velocity;
}

void Object::setVelocity(const sf::Vector2f& vel) {
    velocity = vel;
}

void Object::setCollisionBoxSize(const sf::Vector2f& size) {
    collisionBoxSize = size;
}

sf::Vector2f Object::getCollisionBoxSize() const {
    return collisionBoxSize;
}

cx::Polygon Object::getCollisionPolygon() const {
    cx::Polygon polygon;

    sf::Vector2f halfSize = collisionBoxSize / 2.f;

    sf::Transform collisionTransform;
    collisionTransform.translate(getPosition());
    collisionTransform.rotate(getRotation());

    polygon.points.push_back(collisionTransform.transformPoint({-halfSize.x, -halfSize.y}));
    polygon.points.push_back(collisionTransform.transformPoint({halfSize.x, -halfSize.y}));
    polygon.points.push_back(collisionTransform.transformPoint({halfSize.x, halfSize.y}));
    polygon.points.push_back(collisionTransform.transformPoint({-halfSize.x, halfSize.y}));

    sf::Angle angle = getRotation();
    polygon.axes.push_back({std::cos(angle.asRadians()), std::sin(angle.asRadians())});
    polygon.axes.push_back({-std::sin(angle.asRadians()), std::cos(angle.asRadians())});

    return polygon;
}

void Object::moveAndCollide(const sf::Vector2f& desiredMovement, Slice<cx::Polygon> obstacles) {
    if (desiredMovement.x == 0.f && desiredMovement.y == 0.f) {
        return;
    }

    sf::Vector2f oldPosition = getPosition();

    setPosition({getPosition().x + desiredMovement.x, getPosition().y});
    onPositionUpdate();
    cx::Polygon currentPolygonX = getCollisionPolygon();

    for (const auto& obstacle : obstacles) {
        if (cx::intersects(currentPolygonX, obstacle)) {
            setPosition({oldPosition.x, getPosition().y});
            onPositionUpdate();
            break;
        }
    }

    setPosition({getPosition().x, getPosition().y + desiredMovement.y});
    onPositionUpdate();
    cx::Polygon currentPolygonY = getCollisionPolygon();

    for (const auto& obstacle : obstacles) {
        if (cx::intersects(currentPolygonY, obstacle)) {
            setPosition({getPosition().x, oldPosition.y});
            onPositionUpdate();
            break;
        }
    }
}

} // namespace cx
