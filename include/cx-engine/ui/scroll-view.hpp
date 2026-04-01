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
    sf::Vector2f targetScrollOffset;

public:
    sf::Vector2f mouseScrollSpeed = { 60.f, 60.f };
    sf::Vector2f keyboardScrollSpeed = { 60.f, 60.f };
    sf::Vector2f gamepadScrollSpeed = { 60.f, 60.f };
    Optional<sf::Vector2f> smoothScrollSpeed = std::nullopt;

protected:
    sf::Vector2f getNextScrollOffset(sf::Vector2f current, sf::Vector2f target, float dt) {
        if (!smoothScrollSpeed) return target;
        return {
            current.x + (target.x - current.x) * std::clamp(smoothScrollSpeed->x * dt, 0.f, 1.f),
            current.y + (target.y - current.y) * std::clamp(smoothScrollSpeed->y * dt, 0.f, 1.f),
        };
    }

public:
    ScrollView() : content() {}

    template <typename U>
    ScrollView(U&& c)
        : content(std::forward<U>(c))
        , viewportSize(content.getSize())
        , scrollOffset(0, 0)
        , targetScrollOffset(0, 0)
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
    sf::Vector2f getSize() const override {
        return viewportSize;
    }
    void setUiScale(float scale) override {
        content.setUiScale(scale);
    }

    void setScroll(sf::Vector2f offset) {
        scrollOffset = offset;
        targetScrollOffset = offset;
    }
    sf::Vector2f getScroll() const {
        return scrollOffset;
    }

    void setTargetScroll(sf::Vector2f targetOffset) {
        targetScrollOffset = targetOffset;
    }
    sf::Vector2f getTargetScroll() const {
        return targetScrollOffset;
    }

    void scrollRelative(sf::Vector2f delta) {
        targetScrollOffset += delta;
    }

    void setTargetScrollVertical(float offset)   { setTargetScroll({ targetScrollOffset.x, offset }); }
    void setTargetScrollHorizontal(float offset) { setTargetScroll({ offset, targetScrollOffset.y }); }
    void scrollRelativeVertical(float delta)     { scrollRelative({ 0.f, delta }); }
    void scrollRelativeHorizontal(float delta)   { scrollRelative({ delta, 0.f }); }

    void gamepad(Gamepad& gamepad) override {
        content.gamepad(gamepad);
    }

    void update(float dt) override {
        scrollOffset = getNextScrollOffset(scrollOffset, targetScrollOffset, dt);
        content.update(dt);
    }

    void handle(const sf::Event& event) override {
        content.handle(event);
        if (hasFlag(inputMode, InputMode::Mouse)) {
            if (auto mws = event.getIf<sf::Event::MouseWheelScrolled>()) {
                switch (mws->wheel) {
                case sf::Mouse::Wheel::Vertical:
                    scrollRelativeVertical(-mws->delta * mouseScrollSpeed.y);
                    break;
                case sf::Mouse::Wheel::Horizontal:
                    scrollRelativeHorizontal(-mws->delta * mouseScrollSpeed.x);
                    break;
                }
            }
        }
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
};

} // namespace cx::ui

