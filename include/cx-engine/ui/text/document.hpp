#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Color.hpp>

namespace cx::ui {

struct TextStyle {
    sf::Color color;
    uint size;
    bool bold;
    bool italic;
};

struct TextSpan {
    StringView text;
    TextStyle style;
};

struct TextDocument {
    Vec<TextSpan> spans;
};

} // namespace cx::ui
