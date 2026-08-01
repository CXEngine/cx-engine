#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace cx {

class Entity;
class EntityComponent {
protected:
    Entity& parent;
    bool enabled;

public:
    EntityComponent(Entity& parent) : parent(parent), enabled(true) {}
    virtual ~EntityComponent() = default;

    virtual void update(float dt) {}
    virtual void handle(const sf::Event& event) {}

    virtual void drawBefore(
        sf::RenderTarget& target,
        sf::RenderStates states = sf::RenderStates::Default
    ) const {}
    virtual void drawMiddle(
        sf::RenderTarget& target,
        sf::RenderStates states = sf::RenderStates::Default
    ) const {}
    virtual void drawAfter(
        sf::RenderTarget& target,
        sf::RenderStates states = sf::RenderStates::Default
    ) const {}

    virtual void onPositionUpdate() {}

    virtual void setPosition(const sf::Vector2f& position) {}
    virtual void setRotation(sf::Angle angle) {}
    virtual void setScale(const sf::Vector2f& factors) {}
    virtual void setOrigin(const sf::Vector2f& origin) {}

    virtual void setEnabled(bool isEnabled) { enabled = isEnabled; }
    virtual bool isEnabled() const { return enabled; }
};

}
