#pragma once

#include <cx-engine/utils/hybrid-str.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

namespace cx::ui {

struct TextStyle {
    RefWrapper<const sf::Font> font;
    uint size;

    sf::Color color;
    bool bold;
    bool italic;
};

struct TextSpan {
    HybridStr text;
    TextStyle style;
};

struct TextDocument {
    Vec<TextSpan> spans;
};

} // namespace cx::ui
