#pragma once

#include "cx-engine/systems/gamepad/gamepad.hpp"
#include <cx-engine/utils/bitflags.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

namespace cx {

enum class UiInputMode {
    Mouse = 1 << 0,
    Keyboard = 1 << 1,
    Gamepad = 1 << 2,

    None = 0,
    All = Mouse | Keyboard | Gamepad,
};

TD_BITFLAGS(UiInputMode);

class UiWidget: public sf::Drawable, public sf::Transformable {
protected:
    UiInputMode inputMode;
    bool isFocused;

public:
    virtual ~UiWidget() = default;

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

    void setInputMode(UiInputMode mode) { inputMode = mode; }
    UiInputMode getInputMode() const { return inputMode; }

    virtual void setUiScale(float scale) = 0;
};

} // namespace cx
