#include <cx-engine/systems/gamepad/gamepad-icons.hpp>

#include <cx-engine/systems/assets/cxta.hpp>

#include <stdexcept>

namespace cx {

void GamepadIcons::load(const TextureAtlas& xbox, const TextureAtlas& ps) {
    xboxAtlas = xbox;
    psAtlas = ps;
}

String GamepadIcons::buttonToString(PadButton button) const {
    switch (button) {
    case PadButton::South:     return "south";
    case PadButton::East:      return "east";
    case PadButton::West:      return "west";
    case PadButton::North:     return "north";
    case PadButton::L1:        return "L1";
    case PadButton::R1:        return "R1";
    case PadButton::L2:        return "L2";
    case PadButton::R2:        return "R2";
    case PadButton::L3:        return "L3";
    case PadButton::R3:        return "R3";
    case PadButton::Select:    return "select";
    case PadButton::Start:     return "start";
    case PadButton::DpadUp:    return "dpad-up";
    case PadButton::DpadDown:  return "dpad-down";
    case PadButton::DpadLeft:  return "dpad-left";
    case PadButton::DpadRight: return "dpad-right";
    case PadButton::_Count:
        break;
    }
    throw std::runtime_error("Invalid PadButton for buttonToString");
}

Optional<sf::Sprite> GamepadIcons::getIcon(const Gamepad& gamepad, PadButton button) const {
    const String buttonName = buttonToString(button);
    switch (gamepad.type) {
    case Gamepad::Type::Xbox:
        return xboxAtlas.getSprite(buttonName);
    case Gamepad::Type::PlayStation:
        return psAtlas.getSprite(buttonName);
    case Gamepad::Type::Unknown:
    default:
        return std::nullopt;
    }
}

Optional<sf::Sprite> GamepadIcons::getIcon(const GamepadManager& manager, PadButton button) const {
    if (!manager.hasPrimary()) {
        return std::nullopt;
    }
    const Gamepad& primary = manager.get();
    return getIcon(primary, button);
}

const sf::Texture& GamepadIcons::getAtlas(Gamepad::Type type) const {
    switch (type) {
        case Gamepad::Type::Xbox:
            return xboxAtlas.getTexture();
        case Gamepad::Type::PlayStation:
            return psAtlas.getTexture();
        case Gamepad::Type::Unknown:
        default:
            throw std::runtime_error("Unknown gamepad type has no atlas");
    }
}

const sf::Texture& GamepadIcons::getAtlas(const Gamepad& gamepad) const {
    return getAtlas(gamepad.type);
}

} // namespace cx
