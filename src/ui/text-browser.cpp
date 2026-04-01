#include <cx-engine/ui/text-browser.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <algorithm>

namespace cx::ui {

TextBrowser::TextBrowser() {
    inputMode = InputMode::None;
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

void TextBrowser::setWrapMode(TextWrap mode) {
    wrapMode = mode;
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

static float getTextWidth(const sf::Text& text) {
    const auto& glyphs = text.getShapedGlyphs();
    if (glyphs.empty()) return 0.f;
    const auto& last = glyphs.back();
    return last.position.x + last.glyph.advance;
}

void TextBrowser::appendText(
    const TextSpan& span, StringView t, u32 style,
    float* currentX, float* currentY, float* currentLineMaxHeight
) {
    if (t.empty()) return;

    if (wrapMode == TextWrap::NoWrap || maxWidth <= 0) {
        sf::Text textNode = makeTextNode(span, t, style);
        float width = getTextWidth(textNode);
        float height = span.style.font.get().getLineSpacing((uint)(span.style.size * uiScale));

        renderedSpans.push_back({
            .text = std::move(textNode),
            .position = sf::Vector2f { *currentX, *currentY }
        });

        *currentX += width;
        *currentLineMaxHeight = std::max(*currentLineMaxHeight, height);
        totalSize.x = std::max(totalSize.x, *currentX);
        return;
    }

    if (wrapMode == TextWrap::CharWrap) {
        appendCharByChar(span, t, style, currentX, currentY, currentLineMaxHeight);
        return;
    }

    sf::Text textNode = makeTextNode(span, t, style);
    float width = getTextWidth(textNode);
    float height = span.style.font.get().getLineSpacing((uint)(span.style.size * uiScale));

    if (*currentX + width > maxWidth) {
        if (*currentX > 0) {
            advanceLine(currentX, currentY, currentLineMaxHeight);
            
            width = getTextWidth(textNode);
            if (wrapMode == TextWrap::WordWrapAnywhere && width > maxWidth) {
                appendCharByChar(span, t, style, currentX, currentY, currentLineMaxHeight);
                return;
            }
        } else if (wrapMode == TextWrap::WordWrapAnywhere) {
            appendCharByChar(span, t, style, currentX, currentY, currentLineMaxHeight);
            return;
        }
    }

    renderedSpans.push_back({
        .text = std::move(textNode),
        .position = sf::Vector2f { *currentX, *currentY }
    });

    *currentX += width;
    *currentLineMaxHeight = std::max(*currentLineMaxHeight, height);
    totalSize.x = std::max(totalSize.x, *currentX);
}

void TextBrowser::appendCharByChar(
    const TextSpan& span, StringView t, u32 style,
    float* currentX, float* currentY, float* currentLineMaxHeight
) {
    sf::Text textNode = makeTextNode(span, "", style);
    float height = span.style.font.get().getLineSpacing((uint)(span.style.size * uiScale));

    sf::String currentString;
    sf::String utf8Input = sf::String::fromUtf8(t.begin(), t.end());

    for (const auto& glyphCode : utf8Input) {
        sf::String nextString = currentString + glyphCode;
        textNode.setString(nextString);
        float nextWidth = getTextWidth(textNode);

        if (maxWidth > 0 && (*currentX + nextWidth) > maxWidth) {
            if (!currentString.isEmpty()) {
                textNode.setString(currentString);
                float currentWidth = getTextWidth(textNode);
                renderedSpans.push_back({
                    .text = textNode,
                    .position = sf::Vector2f { *currentX, *currentY }
                });
                *currentX += currentWidth;
                totalSize.x = std::max(totalSize.x, *currentX);
                *currentLineMaxHeight = std::max(*currentLineMaxHeight, height);
                
                advanceLine(currentX, currentY, currentLineMaxHeight);
                currentString = glyphCode;
            } else if (*currentX > 0) {
                advanceLine(currentX, currentY, currentLineMaxHeight);
                currentString = glyphCode;
            } else {
                // character doesn't fit at all, but we must append it
                currentString = nextString;
            }
        } else {
            currentString = nextString;
        }
    }

    if (!currentString.isEmpty()) {
        textNode.setString(currentString);
        float finalWidth = getTextWidth(textNode);
        renderedSpans.push_back({
            .text = std::move(textNode),
            .position = sf::Vector2f { *currentX, *currentY }
        });
        *currentX += finalWidth;
        totalSize.x = std::max(totalSize.x, *currentX);
        *currentLineMaxHeight = std::max(*currentLineMaxHeight, height);
    }
}

void TextBrowser::processSpan(
    const TextSpan& span,
    float* currentX, float* currentY, float* currentLineMaxHeight
) {
    u32 style = (u32) sf::Text::Regular;
    if (span.style.bold)   style |= (u32) sf::Text::Bold;
    if (span.style.italic) style |= (u32) sf::Text::Italic;

    StringView text = span.text.get();

    if (wrapMode == TextWrap::CharWrap) {
        usize start = 0;
        for (usize i = 0; i <= text.size(); ++i) {
            if (i == text.size() || text[i] == '\n') {
                if (i > start) {
                    appendText(span, text.substr(start, i - start), style, currentX, currentY, currentLineMaxHeight);
                }
                if (i < text.size() && text[i] == '\n') {
                    advanceLine(currentX, currentY, currentLineMaxHeight);
                    *currentLineMaxHeight = span.style.font.get().getLineSpacing((uint) (span.style.size * uiScale));
                }
                start = i + 1;
            }
        }
        return;
    }

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

    for (const auto& span: document.spans) {
        processSpan(span, &currentX, &currentY, &currentLineMaxHeight);
    }

    totalSize.y = currentY + currentLineMaxHeight;
}

} // namespace cx::ui
