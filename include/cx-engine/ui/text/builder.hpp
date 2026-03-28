#pragma once

#include <cx-engine/ui/text/document.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx::ui {

class TextDocumentBuilder {
protected:
    TextStyle currentStyle;
    TextDocument output;

public:
    TextDocumentBuilder(TextStyle initialStyle)
        : currentStyle(initialStyle)
    {}

    TextDocumentBuilder& setStyle(TextStyle style) {
        currentStyle = style;
        return *this;
    }
    TextDocumentBuilder& setFont(const sf::Font& font) {
        currentStyle.font = font;
        return *this;
    }
    TextDocumentBuilder& setColor(sf::Color color) {
        currentStyle.color = color;
        return *this;
    }
    TextDocumentBuilder& setSize(uint size) {
        currentStyle.size = size;
        return *this;
    }
    TextDocumentBuilder& setItalic(bool enabled) {
        currentStyle.italic = enabled;
        return * this;
    }
    TextDocumentBuilder& setBold(bool enabled) {
        currentStyle.bold = enabled;
        return *this;
    }

    TextDocumentBuilder& addPart(HybridStr text) {
        output.spans.push_back({ .text = std::move(text), .style = currentStyle });
        return *this;
    }
    TextDocumentBuilder& addPart(HybridStr text, TextStyle override) {
        output.spans.push_back({ .text = std::move(text), .style = override });
        return *this;
    }

    TextDocument build() const& {
        return output;
    }
    TextDocument build() && {
        return std::move(output);
    }
};

} // namespace cx::ui
