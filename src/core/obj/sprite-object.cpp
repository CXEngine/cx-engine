#include <cx-engine/core/obj/sprite-object.hpp>

namespace cx {

SpriteObject::SpriteObject(const sf::Texture& texture, const sf::IntRect& rect)
    : sprite(texture, rect)
{}

void SpriteObject::setPosition(const sf::Vector2f& position) {
    Object::setPosition(position);
    sprite.setPosition(position);
}

void SpriteObject::setRotation(sf::Angle angle) {
    Object::setRotation(angle);
    sprite.setRotation(angle);
}

void SpriteObject::setScale(const sf::Vector2f& factors) {
    Object::setScale(factors);
    sprite.setScale(factors);
}

void SpriteObject::setOrigin(const sf::Vector2f& origin) {
    Object::setOrigin(origin);
    sprite.setOrigin(origin);
}

void SpriteObject::onPositionUpdate() {
    sprite.setPosition(getPosition());
}

} // namespace cx
