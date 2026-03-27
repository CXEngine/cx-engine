#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

namespace cx {

using Key = sf::Keyboard::Key;
constexpr uint KeyCount = sf::Keyboard::KeyCount;

class Keyboard {
public:
    static void init();
    static void update();
    static void handle(const sf::Event& event);

    static bool isPressed(Key key);
    static bool justPressed(Key key);
    static bool justReleased(Key key);

private:
    static bool currentKeyStates[KeyCount];
    static bool previousKeyStates[KeyCount];
};

}
