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
    ScrollView() : content() {}

    template <typename U>
    ScrollView(U&& content)
        : content(std::forward<U>(content)) {}

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

    sf::Vector2f getSize() const override {
        return content.getSize();
    }
    void setUiScale(float scale) override {
        content.setUiScale(scale);
    }
};

} // namespace cx::ui


