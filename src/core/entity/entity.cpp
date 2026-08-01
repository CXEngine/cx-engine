#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/core/entity/components/sprite.hpp>

#include <cx-engine/systems/logging/logger.hpp>

namespace cx {

Entity::Entity(Object base)
    : Object(base)
{}

Entity::Entity(const sf::Texture& texture, const sf::IntRect& rect) {
    addComponent<SpriteComponent>(texture, rect);
}

SpriteComponent& Entity::sprite() {
    if (auto* comp = getComponent<SpriteComponent>())
        return *comp;
    Logger::err("sprite() called on a entity without sprite component");
    std::abort();

}
const SpriteComponent& Entity::sprite() const {
    if (auto* comp = getComponent<SpriteComponent>())
        return *comp;
    Logger::err("sprite() called on a entity without sprite component");
    std::abort();
}

void Entity::onPositionUpdate() {
    Object::onPositionUpdate();
    for (auto& [_, comp] : components) if (comp->isEnabled()) comp->onPositionUpdate();
}
void Entity::setPosition(const sf::Vector2f& position) {
    Object::setPosition(position);
    for (auto& [_, comp]: components) if (comp->isEnabled()) comp->setPosition(position);
}
void Entity::setRotation(sf::Angle angle) {
    Object::setRotation(angle);
    for (auto& [_, comp]: components) if (comp->isEnabled()) comp->setRotation(angle);
}
void Entity::setScale(const sf::Vector2f& factors) {
    Object::setScale(factors);
    for (auto& [_, comp]: components) if (comp->isEnabled()) comp->setScale(factors);
}
void Entity::setOrigin(const sf::Vector2f& origin) {
    Object::setOrigin(origin);
    for (auto& [_, comp]: components) if (comp->isEnabled()) comp->setOrigin(origin);
}

void Entity::update(float dt) {
    Object::update(dt);
    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->update(dt);
        }
    }
}

void Entity::handle(const sf::Event& event) {
    Object::handle(event);
    for (auto& [_, component]: components) {
        if (component->isEnabled()) {
            component->handle(event);
        }
    }
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto drawPhase = [&](auto func) {
        for (auto& [_, component] : components) {
            if (component->isEnabled()) {
                (component.get()->*func)(target, states);
            }
        }
    };

    drawPhase(&EntityComponent::drawBefore);
    drawPhase(&EntityComponent::drawMiddle);
    drawPhase(&EntityComponent::drawAfter);
}

} // namespace cx
