#include <cx-engine/systems/input/keyboard.hpp>

#include <cx-engine/defs/types.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <cstring>

namespace cx {

bool Keyboard::currentKeyStates[KeyCount];
bool Keyboard::previousKeyStates[KeyCount];

void Keyboard::init() {
    std::memset(currentKeyStates, 0, sizeof(currentKeyStates));
    std::memset(previousKeyStates, 0, sizeof(previousKeyStates));
}

void Keyboard::update() {
    std::memcpy(previousKeyStates, currentKeyStates, sizeof(currentKeyStates));
}

void Keyboard::handle(const sf::Event& event) {
    if (auto pressed = event.getIf<sf::Event::KeyPressed>()) {
        currentKeyStates[(uint) pressed->code] = true;
    }
    if (auto released = event.getIf<sf::Event::KeyReleased>()) {
        currentKeyStates[(uint) released->code] = false;
    }
}

bool Keyboard::isPressed(Key key) {
    uint keyAsInt = (uint) key;
    if (keyAsInt >= 0 && keyAsInt < (uint) KeyCount) {
        return currentKeyStates[keyAsInt];
    }
    return false;
}

bool Keyboard::justPressed(Key key) {
    uint keyAsInt = (uint) key;
    if (keyAsInt >= 0 && keyAsInt < (uint) KeyCount) {
        return currentKeyStates[keyAsInt] && !previousKeyStates[keyAsInt];
    }
    return false;
}

bool Keyboard::justReleased(Key key) {
    uint keyAsInt = (uint) key;
    if (keyAsInt >= 0 && keyAsInt < (uint) KeyCount) {
        return !currentKeyStates[keyAsInt] && previousKeyStates[keyAsInt];
    }
    return false;
}

} // namespace cx
