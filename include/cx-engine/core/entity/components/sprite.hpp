#pragma once

#include <cx-engine/core/entity/component.hpp>
#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace cx {

class SpriteComponent: public EntityComponent {
protected:
    sf::Sprite sprite;

public:
    SpriteComponent(Entity& parent, sf::Sprite sprite) : EntityComponent(parent), sprite(std::move(sprite)) {}
    SpriteComponent(Entity& parent, const sf::Texture& texture, const sf::IntRect& rect)
      : EntityComponent(parent)
      , sprite(texture, rect)
    {}

    void drawMiddle(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(sprite, states);
    }

    void setPosition(const sf::Vector2f& position) override { sprite.setPosition(position); }
    void setRotation(sf::Angle angle) override              { sprite.setRotation(angle); }
    void setScale(const sf::Vector2f& factors) override     { sprite.setScale(factors); }
    void setOrigin(const sf::Vector2f& origin) override     { sprite.setOrigin(origin); }

    void onPositionUpdate() override { sprite.setPosition(parent.getPosition()); };

    void setTexture(const sf::Texture& texture, bool resetRect = false) { sprite.setTexture(texture, resetRect); }
    void setTextureRect(const sf::IntRect& rect) { sprite.setTextureRect(rect); }
    void setColor(const sf::Color& color) { sprite.setColor(color); }

    const sf::Texture& getTexture() const { return sprite.getTexture(); }
    sf::IntRect getTextureRect() const { return sprite.getTextureRect(); }
    sf::Color getColor() const { return sprite.getColor(); }

    sf::FloatRect getLocalBounds() const { return sprite.getLocalBounds(); }
    sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

    sf::Sprite& getSprite() { return sprite; }
    const sf::Sprite& getSprite() const { return sprite; }
};

}
