#include <cx-engine/systems/gamepad/gamepad.hpp>
#include <SFML/System/Time.hpp>
#include <algorithm>
#include <cmath>

namespace cx {

static float applyDeadZone(float v, float dz = 15.f) {
    if (std::abs(v) < dz) return 0.f;

    float sign = v > 0.f ? 1.f : -1.f;
    float value = std::abs(v);
    return sign * (value - dz) / (100.f - dz);
}

const std::array<PadButton, static_cast<usize>(PadButton::_Count)> Gamepad::allButtons = {
    PadButton::South, PadButton::East, PadButton::West, PadButton::North,
    PadButton::L1, PadButton::R1, PadButton::L2, PadButton::R2,
    PadButton::L3, PadButton::R3,
    PadButton::Select, PadButton::Start,
    PadButton::DpadUp, PadButton::DpadDown, PadButton::DpadLeft, PadButton::DpadRight
};

Gamepad::Gamepad() : id(sf::Joystick::Count), type(Type::Unknown), connected(false) {
    scrollClock.restart();
}

Gamepad::Gamepad(unsigned int id) {
    scrollClock.restart();
    connect(id);
}

void Gamepad::disconnect() {
    connected = false;
    name = "Disconnected";
    type = Type::Unknown;
    buttons.clear();
    axes.clear();
    for (PadButton b : allButtons) {
        currentButtonStates[b] = false;
        lastButtonStates[b] = false;
    }
}

void Gamepad::connect(unsigned int id) {
    this->id = id;
    if (!sf::Joystick::isConnected(id)) {
        disconnect();
        return;
    }

    connected = true;
    sf::Joystick::update();
    name = sf::Joystick::getIdentification(id).name.toAnsiString();
    detectType();

    for (PadButton b : allButtons) {
        currentButtonStates[b] = false;
        lastButtonStates[b] = false;
    }
}

void Gamepad::detectType() {
    String lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (lowerName.find("xbox") != String::npos) {
        type = Type::Xbox;
        loadXbox();
    } else if (lowerName.find("playstation") != String::npos
            || lowerName.find("ps4") != String::npos
            || lowerName.find("ps5") != String::npos
            || lowerName.find("sony") != String::npos) {
        type = Type::PlayStation;
        loadPlayStation();
    } else {
        type = Type::Unknown;
        loadXbox();
    }
}

void Gamepad::update() {
    if (!connected) return;

    for (PadButton b : allButtons) {
        lastButtonStates[b] = currentButtonStates[b];
        currentButtonStates[b] = queryPressed(b);
    }
}

bool Gamepad::pressed(PadButton button) const {
    auto it = currentButtonStates.find(button);
    return it != currentButtonStates.end() ? it->second : false;
}

bool Gamepad::justPressed(PadButton button) const {
    auto it = currentButtonStates.find(button);
    auto it_last = lastButtonStates.find(button);
    if (it == currentButtonStates.end() || it_last == lastButtonStates.end()) {
        return false;
    }
    return it->second && !it_last->second;
}

bool Gamepad::justReleased(PadButton button) const {
    auto it = currentButtonStates.find(button);
    auto it_last = lastButtonStates.find(button);
    if (it == currentButtonStates.end() || it_last == lastButtonStates.end()) {
        return false;
    }
    return !it->second && it_last->second;
}

bool Gamepad::queryPressed(PadButton button) const {
    if (isDpad(button)) {
        return dpadPressed(button);
    }

    auto it = buttons.find(button);
    if (it == buttons.end()) return false;

    return sf::Joystick::isButtonPressed(id, it->second);
}

float Gamepad::axis(PadAxis axis) const {
    if (type == Type::Xbox && (axis == PadAxis::L2 || axis == PadAxis::R2)) {
        auto it_l = axes.find(PadAxis::L2);
        auto it_r = axes.find(PadAxis::R2);
        if (it_l != axes.end() && it_r != axes.end() && it_l->second == it_r->second) {
            float v = sf::Joystick::getAxisPosition(id, it_l->second);
            if (axis == PadAxis::L2) {
                return (v > 0) ? applyDeadZone(v, 0.f) : 0.f;
            } else { // R2
                return (v < 0) ? applyDeadZone(-v, 0.f) : 0.f;
            }
        }
    }

    auto it = axes.find(axis);
    if (it == axes.end()) return 0.f;

    float v = sf::Joystick::getAxisPosition(id, it->second);

    if (axis == PadAxis::L2 || axis == PadAxis::R2) {
        return (v + 100.f) / 200.f;
    }

    return applyDeadZone(v);
}

void Gamepad::loadXbox() {
    buttons = {
        {PadButton::South, 0},
        {PadButton::East, 1},
        {PadButton::West, 2},
        {PadButton::North, 3},
        {PadButton::L1, 4},
        {PadButton::R1, 5},
        {PadButton::L3, 8},
        {PadButton::R3, 9},
        {PadButton::Select, 6},
        {PadButton::Start, 7},
    };

    axes = {
        {PadAxis::LeftX, sf::Joystick::Axis::X},
        {PadAxis::LeftY, sf::Joystick::Axis::Y},
        {PadAxis::RightX, sf::Joystick::Axis::U},
        {PadAxis::RightY, sf::Joystick::Axis::V},
        #if defined(_WIN32)
        // On Windows, L2 and R2 are on the same Z axis
        {PadAxis::L2, sf::Joystick::Axis::Z},
        {PadAxis::R2, sf::Joystick::Axis::Z},
        #else
        // On Linux/macOS, they are typically separate axes
        {PadAxis::L2, sf::Joystick::Axis::Z},
        {PadAxis::R2, sf::Joystick::Axis::R},
        #endif
    };
}

void Gamepad::loadPlayStation() {
    buttons = {
        {PadButton::South, 0},
        {PadButton::East, 1},
        {PadButton::West, 3},
        {PadButton::North, 2},
        {PadButton::L1, 4},
        {PadButton::R1, 5},
        {PadButton::L2, 6},
        {PadButton::R2, 7},
        {PadButton::L3, 11},
        {PadButton::R3, 12},
        {PadButton::Select, 8},
        {PadButton::Start, 9},
    };

    axes = {
        {PadAxis::LeftX, sf::Joystick::Axis::X},
        {PadAxis::LeftY, sf::Joystick::Axis::Y},
        {PadAxis::RightX, sf::Joystick::Axis::U},
        {PadAxis::RightY, sf::Joystick::Axis::V},
        {PadAxis::L2, sf::Joystick::Axis::Z},
        {PadAxis::R2, sf::Joystick::Axis::R}
    };
}

bool Gamepad::isDpad(PadButton b) const {
    return b == PadButton::DpadUp ||
           b == PadButton::DpadDown ||
           b == PadButton::DpadLeft ||
           b == PadButton::DpadRight;
}

bool Gamepad::dpadPressed(PadButton b) const {
    float x = sf::Joystick::getAxisPosition(id, sf::Joystick::Axis::PovX);
    float y = sf::Joystick::getAxisPosition(id, sf::Joystick::Axis::PovY);

    if (b == PadButton::DpadLeft)  return x < -50.f;
    if (b == PadButton::DpadRight) return x > 50.f;
    if (b == PadButton::DpadUp)    return y > -50.f;
    if (b == PadButton::DpadDown)  return y < 50.f;

    return false;
}

bool Gamepad::isLeftStickUp(float threshold) const { return axis(PadAxis::LeftY) < -threshold; }
bool Gamepad::isLeftStickDown(float threshold) const { return axis(PadAxis::LeftY) > threshold; }
bool Gamepad::isLeftStickLeft(float threshold) const { return axis(PadAxis::LeftX) < -threshold; }
bool Gamepad::isLeftStickRight(float threshold) const { return axis(PadAxis::LeftX) > threshold; }

bool Gamepad::isRightStickUp(float threshold) const { return axis(PadAxis::RightY) < -threshold; }
bool Gamepad::isRightStickDown(float threshold) const { return axis(PadAxis::RightY) > threshold; }
bool Gamepad::isRightStickLeft(float threshold) const { return axis(PadAxis::RightX) < -threshold; }
bool Gamepad::isRightStickRight(float threshold) const {
    return axis(PadAxis::RightX) > threshold;
}

bool Gamepad::justScrolledUp(float threshold) {
    if (justPressed(PadButton::DpadUp)) return true;
    if (isLeftStickUp(threshold)) {
        float elapsed = scrollClock.getElapsedTime().asSeconds();
        if (elapsed - lastScrollTimes[0] > scrollRepeatDelay) {
            lastScrollTimes[0] = elapsed;
            return true;
        }
    } else {
        lastScrollTimes[0] = 0;
    }
    return false;
}

bool Gamepad::justScrolledDown(float threshold) {
    if (justPressed(PadButton::DpadDown)) return true;
    if (isLeftStickDown(threshold)) {
        float elapsed = scrollClock.getElapsedTime().asSeconds();
        if (elapsed - lastScrollTimes[1] > scrollRepeatDelay) {
            lastScrollTimes[1] = elapsed;
            return true;
        }
    } else {
        lastScrollTimes[1] = 0;
    }
    return false;
}

bool Gamepad::justScrolledLeft(float threshold) {
    if (justPressed(PadButton::DpadLeft)) return true;
    if (isLeftStickLeft(threshold)) {
        float elapsed = scrollClock.getElapsedTime().asSeconds();
        if (elapsed - lastScrollTimes[2] > scrollRepeatDelay) {
            lastScrollTimes[2] = elapsed;
            return true;
        }
    } else {
        lastScrollTimes[2] = 0;
    }
    return false;
}

bool Gamepad::justScrolledRight(float threshold) {
    if (justPressed(PadButton::DpadRight)) return true;
    if (isLeftStickRight(threshold)) {
        float elapsed = scrollClock.getElapsedTime().asSeconds();
        if (elapsed - lastScrollTimes[3] > scrollRepeatDelay) {
            lastScrollTimes[3] = elapsed;
            return true;
        }
    } else {
        lastScrollTimes[3] = 0;
    }
    return false;
}

} // namespace cx
