#include <SFML/System/String.hpp>
#include <cx-engine/utils/wrap-text.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx {

sf::String wrapText(
    const sf::String& text,
    const sf::Font& font,
    unsigned int characterSize,
    float maxWidth
) {
    sf::Text measure(font, "", characterSize);

    sf::String result;
    sf::String line;
    sf::String word;

    auto flushWord = [&] {
        if (word.isEmpty())
            return;

        sf::String test = line;
        if (!line.isEmpty())
            test += ' ';
        test += word;

        measure.setString(test);

        if (measure.getLocalBounds().size.x > maxWidth) {
            if (!line.isEmpty())
                result += line + '\n';
            line = word;
        } else {
            if (!line.isEmpty())
                line += ' ';
            line += word;
        }

        word.clear();
    };

    for (usize i = 0; i < text.getSize(); ++i) {
        char32_t c = text[i];

        if (c == ' ') {
            flushWord();
        } else if (c == '\n') {
            flushWord();
            result += line + '\n';
            line.clear();
        } else {
            word += c;
        }
    }

    flushWord();
    if (!line.isEmpty())
        result += line;

    return result;
}

} // namespace cx
