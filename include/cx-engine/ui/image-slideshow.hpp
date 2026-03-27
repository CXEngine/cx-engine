#pragma once

#include <SFML/Graphics.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx::ui {

class ImageSlideshow: public sf::Drawable {
private:
    Slice<sf::Texture> textures;

    sf::Sprite current;
    sf::Sprite next;

    float displayTime;
    float transitionTime;
    float timer;

    sf::Vector2f pos;
    sf::Vector2f size;

    int currentIndex;
    int nextIndex;

    enum class Phase {
        Display,
        Transition
    };

    Phase phase;

    void pickNext();
    void fitSpriteCover(sf::Sprite& sprite);

public:
    ImageSlideshow(float displaySeconds,
                   float transitionSeconds,
                   sf::Vector2f pos,
                   sf::Vector2f size,
                   Slice<sf::Texture> textures);

    void setPosition(sf::Vector2f position);
    void setSize(sf::Vector2f newSize);
    void update(float dt);

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;
};

} // namespace cx
