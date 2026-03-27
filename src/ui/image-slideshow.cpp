#include <cx-engine/ui/image-slideshow.hpp>
#include <algorithm>
#include <cstdlib>

namespace cx::ui {

ImageSlideshow::ImageSlideshow(
    float displaySeconds,
    float transitionSeconds,
    sf::Vector2f pos,
    sf::Vector2f size,
    Slice<sf::Texture> textures
)
    : textures(textures)
    , current(textures[0])
    , next(textures[0])
    , displayTime(displaySeconds)
    , transitionTime(transitionSeconds)
    , timer(0.f)
    , pos(pos)
    , size(size)
    , currentIndex(0)
    , nextIndex(0)
    , phase(Phase::Display)
{
    nextIndex = rand() % (int)textures.size();
    pickNext();
    setPosition(pos);
    setSize(size);
}

void ImageSlideshow::pickNext() {
    currentIndex = nextIndex;

    if (textures.size() > 1) {
        do {
            nextIndex = rand() % (int)textures.size();
        } while (nextIndex == currentIndex);
    } else {
        nextIndex = 0;
    }

    current.setTexture(textures[currentIndex], true);
    next.setTexture(textures[nextIndex], true);

    fitSpriteCover(current);
    fitSpriteCover(next);

    current.setColor(sf::Color::White);
    next.setColor(sf::Color(255, 255, 255, 0));
}

void ImageSlideshow::fitSpriteCover(sf::Sprite& sprite) {
    sf::Vector2u texSize = sprite.getTexture().getSize();
    float scale = std::max(size.x / texSize.x, size.y / texSize.y);

    sprite.setScale({
        scale,
        scale,
    });
    sprite.setOrigin({
        texSize.x * 0.5f,
        texSize.y * 0.5f,
    });
    sprite.setPosition({
        pos.x + size.x * 0.5f,
        pos.y + size.y * 0.5f
    });
}

void ImageSlideshow::setPosition(sf::Vector2f position) {
    pos = position;
    current.setPosition({
        pos.x + size.x * 0.5f,
        pos.y + size.y * 0.5f
    });
    next.setPosition({
        pos.x + size.x * 0.5f,
        pos.y + size.y * 0.5f
    });
}

void ImageSlideshow::setSize(sf::Vector2f newSize) {
    size = newSize;
    fitSpriteCover(current);
    fitSpriteCover(next);
}

void ImageSlideshow::update(float dt) {
    if (textures.size() < 2)
        return;

    timer += dt;

    if (phase == Phase::Display) {
        if (timer >= displayTime) {
            timer = 0.f;
            phase = Phase::Transition;
        }
        return;
    }

    float t = timer / transitionTime;
    if (t > 1.f) t = 1.f;

    current.setColor(sf::Color(255, 255, 255, (byte) (255 * (1.f - t))));
    next.setColor(sf::Color(255, 255, 255, (byte) (255 * t)));

    if (timer >= transitionTime) {
        pickNext();
        timer = 0.f;
        phase = Phase::Display;
    }
}

void ImageSlideshow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(current, states);
    target.draw(next, states);
}

} // namespace cx
