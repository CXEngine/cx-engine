#include <cx-engine/core/entity/entity.hpp>

namespace cx {

Entity::Entity(SpriteObject base)
    : SpriteObject(base)
{}

Entity::Entity(const sf::Texture& texture, const sf::IntRect& rect)
    : SpriteObject(texture, rect)
{}

void Entity::update(float dt) {
    SpriteObject::update(dt);
    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->update(dt);
        }
    }
}

void Entity::handle(const sf::Event& event) {
    SpriteObject::handle(event);
    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->handle(event);
        }
    }
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->drawBefore(target, states);
        }
    }

    SpriteObject::draw(target, states);

    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->drawAfter(target, states);
        }
    }
}

}
