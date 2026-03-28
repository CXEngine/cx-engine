#include <cx-engine/ui/text-browser.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <algorithm>

namespace cx::ui {

TextBrowser::TextBrowser() {
    inputMode = UiInputMode::None;
    isFocused = false;
}

void TextBrowser::setDocument(const TextDocument& doc) {
    document = doc;
    dirty = true;
}
void TextBrowser::setDocument(TextDocument&& doc) {
    document = std::move(doc);
    dirty = true;
}

void TextBrowser::setMaxWidth(float width) {
    maxWidth = width;
    dirty = true;
}

void TextBrowser::setLineSpacing(float spacing) {
    lineSpacing = spacing;
    dirty = true;
}

sf::Vector2f TextBrowser::getSize() const {
    return totalSize;
}

void TextBrowser::setUiScale(float scale) {
    uiScale = scale;
    dirty = true;
}

void TextBrowser::update(float dt) {
    if (dirty) computeLayout();
}

void TextBrowser::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    for (const auto& span: renderedSpans) {
        sf::RenderStates spanStates = states;
        spanStates.transform.translate(span.position);
        target.draw(span.text, spanStates);
    }
}

void TextBrowser::advanceLine(float* currentX, float* currentY, float* currentLineMaxHeight) {
    *currentX = 0;
    *currentY += (*currentLineMaxHeight) * lineSpacing;
    *currentLineMaxHeight = 0;
}

sf::Text TextBrowser::makeTextNode(const TextSpan& span, StringView t, u32 style) {
    sf::Text textNode(span.style.font, sf::String::fromUtf8(t.begin(), t.end()), (uint)(span.style.size * uiScale));
    textNode.setStyle((sf::Text::Style) style);
    textNode.setFillColor(span.style.color);
    return textNode;
}

void TextBrowser::appendText(
    const TextSpan& span, StringView t, u32 style,
    float* currentX, float* currentY, float* currentLineMaxHeight
) {
    if (t.empty()) return;

    sf::Text textNode = makeTextNode(span, t, style);

    float width = textNode.getLocalBounds().size.x;
    float height = span.style.font.get().getLineSpacing((uint)(span.style.size * uiScale));

    if (maxWidth > 0 && (*currentX + width) > maxWidth && *currentX > 0) {
        advanceLine(currentX, currentY, currentLineMaxHeight);
    }

    renderedSpans.push_back({
        .text = std::move(textNode),
        .position = sf::Vector2f { *currentX, *currentY }
    });

    *currentX += width;
    *currentLineMaxHeight = std::max(*currentLineMaxHeight, height);
    totalSize.x = std::max(totalSize.x, *currentX);
}

void TextBrowser::processSpan(
    const TextSpan& span,
    float* currentX, float* currentY, float* currentLineMaxHeight
) {
    u32 style = (u32) sf::Text::Regular;
    if (span.style.bold)   style |= (u32) sf::Text::Bold;
    if (span.style.italic) style |= (u32) sf::Text::Italic;

    StringView text = span.text.get();
    usize wordStart = 0;

    for (usize i = 0; i <= text.size(); ++i) {
        bool isEnd = (i == text.size());
        bool isSpace = !isEnd && (text[i] == ' ');
        bool isNewline = !isEnd && (text[i] == '\n');

        if (isEnd || isSpace || isNewline) {
            if (i > wordStart) {
                appendText(span, text.substr(wordStart, i - wordStart), style,
                           currentX, currentY, currentLineMaxHeight);
            }

            if (isNewline) {
                advanceLine(currentX, currentY, currentLineMaxHeight);
                *currentLineMaxHeight = span.style.font.get().getLineSpacing((uint) (span.style.size * uiScale));
            } else if (isSpace) {
                appendText(span, " ", style, currentX, currentY, currentLineMaxHeight);
            }

            wordStart = i + 1;
        }
    }
}

void TextBrowser::computeLayout() {
    renderedSpans.clear();
    totalSize = { 0, 0 };
    dirty = false;

    float currentX = 0;
    float currentY = 0;
    float currentLineMaxHeight = 0;

    for (const auto& span : document.spans) {
        processSpan(span, &currentX, &currentY, &currentLineMaxHeight);
    }

    totalSize.y = currentY + currentLineMaxHeight;
}

} // namespace cx::ui
