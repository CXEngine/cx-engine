#include <cx-engine/systems/gamepad/gamepad-manager.hpp>
#include <cx-engine/systems/gamepad/error.hpp>

namespace cx {

GamepadIterator::GamepadIterator(Array<Gamepad, sf::Joystick::Count>& gamepads, usize index)
    : container(gamepads), index(index) {
    if (this->index < sf::Joystick::Count && !container[this->index].isConnected()) {
        advanceToNextConnected();
    }
}

GamepadIterator::reference GamepadIterator::operator*() const {
    return container[index];
}

GamepadIterator::pointer GamepadIterator::operator->() const {
    return &container[index];
}

GamepadIterator& GamepadIterator::operator++() {
    advanceToNextConnected();
    return *this;
}

GamepadIterator GamepadIterator::operator++(int) {
    GamepadIterator tmp = *this;
    ++(*this);
    return tmp;
}

void GamepadIterator::advanceToNextConnected() {
    index++;
    while (index < sf::Joystick::Count && !container[index].isConnected()) {
        index++;
    }
}

bool operator==(const GamepadIterator& a, const GamepadIterator& b) {
    return &a.container == &b.container && a.index == b.index;
}

bool operator!=(const GamepadIterator& a, const GamepadIterator& b) {
    return !(a == b);
}

GamepadManager::GamepadManager() : connectedCount(0), primaryGamepadId(-1) {
    scan();
}

GamepadScanResult GamepadManager::scan() {
    GamepadScanResult result;
    sf::Joystick::update();

    connectedCount = 0;
    for (uint i = 0; i < sf::Joystick::Count; ++i) {
        bool isConnectedNow = sf::Joystick::isConnected(i);
        bool wasConnected = gamepads[i].isConnected();

        if (isConnectedNow) {
            if (!wasConnected) {
                gamepads[i].connect(i);
                if (gamepads[i].isConnected()) {
                    result.newlyConnected.push(i);
                }
            }
            if (gamepads[i].isConnected()) {
                connectedCount++;
            }
        } else {
            if (wasConnected) {
                gamepads[i].disconnect();
                result.newlyDisconnected.push(i);
            }
        }
    }

    if (primaryGamepadId != -1 && !gamepads[primaryGamepadId].isConnected()) {
        primaryGamepadId = -1;
    }
    if (primaryGamepadId == -1 && connectedCount > 0) {
        for (uint i = 0; i < sf::Joystick::Count; ++i) {
            if (gamepads[i].isConnected()) {
                primaryGamepadId = i;
                break;
            }
        }
    }

    return result;
}

void GamepadManager::update() {
    sf::Joystick::update();
    for (Gamepad& pad : *this) {
        pad.update();
    }
}

Gamepad& GamepadManager::get(uint id) {
    if (id >= sf::Joystick::Count) {
        throw std::out_of_range("Gamepad ID out of range.");
    }
    return gamepads[id];
}

const Gamepad& GamepadManager::get(uint id) const {
    if (id >= sf::Joystick::Count) {
        throw std::out_of_range("Gamepad ID out of range.");
    }
    return gamepads[id];
}

Gamepad& GamepadManager::get() {
    if (!hasPrimary()) {
        throw GamepadError("No primary gamepad is connected or set.");
    }
    return gamepads[primaryGamepadId];
}

const Gamepad& GamepadManager::get() const {
    if (!hasPrimary()) {
        throw GamepadError("No primary gamepad is connected or set.");
    }
    return gamepads[primaryGamepadId];
}

bool GamepadManager::hasPrimary() const {
    return primaryGamepadId != -1;
}

void GamepadManager::setPrimary(uint id) {
    if (id < sf::Joystick::Count && gamepads[id].isConnected()) {
        primaryGamepadId = id;
    } else {
        throw GamepadError("Cannot set primary to a disconnected gamepad ID.");
    }
}

usize GamepadManager::getConnectedCount() const {
    return connectedCount;
}

GamepadIterator GamepadManager::begin() {
    return GamepadIterator(gamepads, 0);
}

GamepadIterator GamepadManager::end() {
    return GamepadIterator(gamepads, sf::Joystick::Count);
}

} // namespace cx
