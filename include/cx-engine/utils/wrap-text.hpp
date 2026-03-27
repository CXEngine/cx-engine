#pragma once
#include <SFML/Graphics.hpp>

namespace cx {

sf::String wrapText(
    const sf::String& text,
    const sf::Font& font,
    unsigned int characterSize,
    float maxWidth
);

} // namespace cx
