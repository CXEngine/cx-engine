#pragma once

#include <cx-engine/ui/widget.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace cx::ui {

// TODO: template
template <DerivedFromWidget TContent>
class ScrollView: public Widget {
protected:
    TContent content;

public:
    ScrollView(TContent content) : content(std::move(content)) {}

    TContent& setContent(TContent c) {
        return content = std::move(c);
    }

    const TContent& getContent() const { return content; }
    TContent& getContent() { return content; }

    const TContent* operator->() const { return &content; }
    TContent* operator->() { return &content; }

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
        target.draw(content, states);
    }
};

} // namespace cx::ui


