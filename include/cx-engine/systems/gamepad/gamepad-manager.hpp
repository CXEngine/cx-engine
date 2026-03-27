#pragma once

#include <cx-engine/systems/gamepad/gamepad.hpp>

#include <cx-engine/defs/types.hpp>
#include <cx-engine/utils/buf.hpp>

#include <iterator>
#include <stdexcept>

namespace cx {

class GamepadManager;

class GamepadIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Gamepad;
    using difference_type = std::ptrdiff_t;
    using pointer = Gamepad*;
    using reference = Gamepad&;

    GamepadIterator(Array<Gamepad, sf::Joystick::Count>& gamepads, usize index);

    reference operator*() const;
    pointer operator->() const;
    GamepadIterator& operator++();
    GamepadIterator operator++(int);

    friend bool operator==(const GamepadIterator& a, const GamepadIterator& b);
    friend bool operator!=(const GamepadIterator& a, const GamepadIterator& b);

private:
    void advanceToNextConnected();
    Array<Gamepad, MaxGamepads>& container;
    usize index;
};

class GamepadError: public std::runtime_error {
    using runtime_error::runtime_error;
};

struct GamepadScanResult {
    Buf<uint, MaxGamepads> newlyConnected;
    Buf<uint, MaxGamepads> newlyDisconnected;
};

class GamepadManager {
public:
    GamepadManager();
    ~GamepadManager() = default;

    GamepadManager(const GamepadManager&) = delete;
    GamepadManager& operator=(const GamepadManager&) = delete;
    GamepadManager(GamepadManager&&) = default;
    GamepadManager& operator=(GamepadManager&&) = delete;

    GamepadScanResult scan();
    void update();

    Gamepad& get(unsigned int id);
    const Gamepad& get(unsigned int id) const;

    Gamepad& get();
    const Gamepad& get() const;
    bool hasPrimary() const;
    void setPrimary(unsigned int id);

    usize getConnectedCount() const;

    GamepadIterator begin();
    GamepadIterator end();

private:
    Array<Gamepad, sf::Joystick::Count> gamepads;
    usize connectedCount;
    int primaryGamepadId;
};

} // namespace cx
