#pragma once

#include <cx-engine/systems/gamepad/gamepad.hpp>
#include <cx-engine/utils/bitflags.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

namespace cx::ui {

enum class InputMode {
    Mouse = 1 << 0,
    Keyboard = 1 << 1,
    Gamepad = 1 << 2,

    None = 0,
    All = Mouse | Keyboard | Gamepad,
};

TD_BITFLAGS(InputMode);

class Widget: public sf::Drawable, public sf::Transformable {
protected:
    InputMode inputMode;
    bool isFocused;

public:
    virtual ~Widget() = default;

    virtual void gamepad(Gamepad& gamepad) {}
    virtual void handle(const sf::Event& event) {}
    virtual void update(float dt) {}
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override {}

    virtual sf::Vector2f getSize() const = 0;

    virtual sf::FloatRect getGlobalBounds() const {
        return { getPosition(), getSize() };
    }

    void setFocus(bool f) { isFocused = f; }
    bool hasFocus() const { return isFocused; }

    void setInputMode(InputMode mode) { inputMode = mode; }
    InputMode getInputMode() const { return inputMode; }

    virtual void setUiScale(float scale) = 0;
};

template <typename T>
concept DerivedFromWidget = std::derived_from<T, Widget>;

} // namespace cx::ui
