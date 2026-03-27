#pragma once

#include <cx-engine/core/obj/object.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace cx {

class SpriteObject: public Object {
public:
    SpriteObject(const sf::Texture& texture, const sf::IntRect& rect);
    virtual ~SpriteObject() = default;

    virtual void handle(const sf::Event& event) override {}
    virtual void update(float dt) override {}

    void setPosition(const sf::Vector2f& position);
    void setRotation(sf::Angle angle);
    void setScale(const sf::Vector2f& factors);
    void setOrigin(const sf::Vector2f& origin);

    void setTexture(const sf::Texture& texture, bool resetRect = false) { sprite.setTexture(texture, resetRect); }
    void setTextureRect(const sf::IntRect& rect) { sprite.setTextureRect(rect); }
    void setColor(const sf::Color& color) { sprite.setColor(color); }

    const sf::Texture& getTexture() const { return sprite.getTexture(); }
    sf::IntRect getTextureRect() const { return sprite.getTextureRect(); }
    sf::Color getColor() const { return sprite.getColor(); }

    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

    sf::Sprite& getSprite() { return sprite; }
    const sf::Sprite& getSprite() const { return sprite; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(sprite, states);
    }

protected:
    sf::Sprite sprite;

    virtual void onPositionUpdate() override;
};

} // namespace cx
