#pragma once

#include <SFML/Window/WindowBase.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

namespace cx {

using MouseBtn = sf::Mouse::Button;
constexpr uint MouseBtnCount = sf::Mouse::ButtonCount;

class Mouse {
public:
    static void init();
    static void update();
    static void handle(const sf::Event& event);

    static bool isPressed(MouseBtn key);
    static bool justPressed(MouseBtn key);
    static bool justReleased(MouseBtn key);

    static sf::Vector2i getCursorPos();
    static sf::Vector2i getCursorPos(const sf::WindowBase& relativeTo);

private:
    static bool currentBtnStates[MouseBtnCount];
    static bool previousBtnStates[MouseBtnCount];

};

}
