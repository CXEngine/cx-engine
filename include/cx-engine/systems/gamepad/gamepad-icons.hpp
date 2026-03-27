#pragma once

#include <cx-engine/systems/gamepad/gamepad.hpp>
#include <cx-engine/systems/gamepad/gamepad-manager.hpp>

#include <cx-engine/systems/assets/cxpk.hpp>
#include <cx-engine/systems/assets/cxta.hpp>

#include <SFML/Graphics.hpp>

namespace cx {

class GamepadIcons {
public:
    GamepadIcons() = default;

    void load(const TextureAtlas& xbox, const TextureAtlas& ps);

    Optional<sf::Sprite> getIcon(const Gamepad& gamepad, PadButton button) const;
    Optional<sf::Sprite> getIcon(const GamepadManager& manager, PadButton button) const;

    const sf::Texture& getAtlas(Gamepad::Type type) const;
    const sf::Texture& getAtlas(const Gamepad& gamepad) const;

private:
    String buttonToString(PadButton button) const;

    TextureAtlas xboxAtlas;
    TextureAtlas psAtlas;
};

} // namespace cx
