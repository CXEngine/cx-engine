#pragma once

#include <cx-engine/ui/widget.hpp>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

namespace cx::ui {

// TODO: template
template <DerivedFromWidget TContent>
class ScrollView: public Widget {
protected:
    mutable sf::RenderTexture rt;

    TContent content;
    sf::Vector2f viewportSize;
    sf::Vector2f scrollOffset;

public:
    ScrollView() : content() {}

    template <typename U>
    ScrollView(U&& c)
        : content(std::forward<U>(c))
        , viewportSize(content.getSize())
        , scrollOffset(0, 0)
    {}

    template <typename U>
    TContent& setContent(U&& c) {
        return content = std::forward<U>(c);
    }

    template <typename... Args>
    TContent& makeContent(Args&&... args) {
        return content = TContent(std::forward<Args>(args)...);
    }

    const TContent& getContent() const { return content; }
    TContent& getContent() { return content; }

    void setSize(sf::Vector2f size) {
        viewportSize = size;
    }

    void setScroll(sf::Vector2f offset) {
        scrollOffset = offset;
    }
    sf::Vector2f getScroll() const {
        return scrollOffset;
    }
    void scrollRelative(sf::Vector2f delta) {
        setScroll(getScroll() + delta);
    }

    void gamepad(Gamepad& gamepad) override {
        content.gamepad(gamepad);
    }

    void update(float dt) override {
        content.update(dt);
    }

    void handle(const sf::Event& event) override {
        content.handle(event);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        sf::Vector2f size = getSize();
        if (size.x < 1.f || size.y < 1.f) return;

        sf::Vector2u rtSize = {
            (uint) std::ceil(size.x),
            (uint) std::ceil(size.y),
        };

        if (rt.getSize() != rtSize) {
            if (!rt.resize(rtSize)) {
                return;
            }
        }

        rt.clear(sf::Color::Transparent);
        
        sf::RenderStates contentStates;
        
        contentStates.transform.translate(-scrollOffset);
        rt.draw(content, contentStates);
        rt.display();

        states.transform *= getTransform();
        sf::Sprite rs(rt.getTexture());
        target.draw(rs, states);
    }

    sf::Vector2f getSize() const override {
        return viewportSize;
    }

    void setUiScale(float scale) override {
        content.setUiScale(scale);
    }
};

} // namespace cx::ui

