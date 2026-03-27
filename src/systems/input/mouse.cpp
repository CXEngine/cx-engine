#include <SFML/Window/WindowBase.hpp>
#include <cx-engine/systems/input/mouse.hpp>

#include <SFML/System/Vector2.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cstring>

namespace cx {

bool Mouse::currentBtnStates[MouseBtnCount];
bool Mouse::previousBtnStates[MouseBtnCount];

void Mouse::init() {
    std::memset(currentBtnStates, 0, sizeof(currentBtnStates));
    std::memset(previousBtnStates, 0, sizeof(previousBtnStates));
}

void Mouse::update() {
    std::memcpy(previousBtnStates, currentBtnStates, sizeof(currentBtnStates));
}

void Mouse::handle(const sf::Event& event) {
    if (auto pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        currentBtnStates[(uint) pressed->button] = true;
    }
    if (auto released = event.getIf<sf::Event::MouseButtonReleased>()) {
        currentBtnStates[(uint) released->button] = false;
    }
}

bool Mouse::isPressed(MouseBtn btn) {
    uint btnAsInt = (uint) btn;
    if (btnAsInt >= 0 && btnAsInt < (uint) MouseBtnCount) {
        return currentBtnStates[btnAsInt];
    }
    return false;
}

bool Mouse::justPressed(MouseBtn btn) {
    uint btnAsInt = (uint) btn;
    if (btnAsInt >= 0 && btnAsInt < (uint) MouseBtnCount) {
        return currentBtnStates[btnAsInt] && !previousBtnStates[btnAsInt];
    }
    return false;
}

bool Mouse::justReleased(MouseBtn btn) {
    uint btnAsInt = (uint) btn;
    if (btnAsInt >= 0 && btnAsInt < (uint) MouseBtnCount) {
        return !currentBtnStates[btnAsInt] && previousBtnStates[btnAsInt];
    }
    return false;
}

sf::Vector2i Mouse::getCursorPos() {
    return sf::Mouse::getPosition();
}
sf::Vector2i Mouse::getCursorPos(const sf::WindowBase& relativeTo) {
    return sf::Mouse::getPosition(relativeTo);
}

}
