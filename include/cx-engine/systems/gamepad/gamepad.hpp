#pragma once

#include <cx-engine/defs/types.hpp>

#include <SFML/Window/Joystick.hpp>
#include <SFML/System/Clock.hpp>

namespace cx {

using GamepadID = u8;

constexpr GamepadID MaxGamepads = sf::Joystick::Count;

enum class PadButton {
    South,
    East,
    West,
    North,
    L1,
    R1,
    L2,
    R2,
    L3,
    R3,
    Select,
    Start,
    DpadUp,
    DpadDown,
    DpadLeft,
    DpadRight,

    _Count
};

enum class PadAxis {
    LeftX,
    LeftY,
    RightX,
    RightY,
    L2,
    R2,

    _Count
};

class Gamepad {
public:
    enum class Type {
        Unknown,
        Xbox,
        PlayStation
    };


    explicit Gamepad(unsigned int id);
    Gamepad();

    // Connection
    void connect(unsigned int id);
    bool isConnected() const { return connected; }
    void update();
    void disconnect();

    // State
    bool pressed(PadButton button) const;
    bool justPressed(PadButton button) const;
    bool justReleased(PadButton button) const;
    float axis(PadAxis axis) const;

    // UI helpers
    bool isLeftStickUp(float threshold = 0.8f) const;
    bool isLeftStickDown(float threshold = 0.8f) const;
    bool isLeftStickLeft(float threshold = 0.8f) const;
    bool isLeftStickRight(float threshold = 0.8f) const;

    bool isRightStickUp(float threshold = 0.8f) const;
    bool isRightStickDown(float threshold = 0.8f) const;
    bool isRightStickLeft(float threshold = 0.8f) const;
    bool isRightStickRight(float threshold = 0.8f) const;

    bool justScrolledUp(float threshold = 0.8f);
    bool justScrolledDown(float threshold = 0.8f);
    bool justScrolledLeft(float threshold = 0.8f);
    bool justScrolledRight(float threshold = 0.8f);

    unsigned int id;
    String name;
    Type type = Type::Unknown;
    bool connected = false;

private:
    static const Array<PadButton, (usize) PadButton::_Count> allButtons;

    HashMap<PadButton, unsigned int> buttons;
    HashMap<PadAxis, sf::Joystick::Axis> axes;
    HashMap<PadButton, bool> currentButtonStates;
    HashMap<PadButton, bool> lastButtonStates;

    sf::Clock scrollClock;
    float lastScrollTimes[4] = {0.f, 0.f, 0.f, 0.f}; // 0: up, 1: down, 2: left, 3: right
    const float scrollRepeatDelay = 0.2f;

    void detectType();
    void loadXbox();
    void loadPlayStation();

    bool queryPressed(PadButton button) const;
    bool isDpad(PadButton b) const;
    bool dpadPressed(PadButton b) const;
};

} // namespace cx
