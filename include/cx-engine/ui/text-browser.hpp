#pragma once

#include <cx-engine/ui/ui-widget.hpp>
#include <cx-engine/ui/text/document.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

namespace cx::ui {

class TextBrowser : public UiWidget {
public:
    TextBrowser();

    void setDocument(const TextDocument& doc);
    void setDocument(TextDocument&& doc);
    void setMaxWidth(float width);
    void setLineSpacing(float spacing);

    sf::Vector2f getSize() const override;
    void setUiScale(float scale) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    TextDocument document;

    float maxWidth = 0.f;
    float lineSpacing = 1.2f;
    float uiScale = 1.f;

    struct RenderedSpan {
        sf::Text text;
        sf::Vector2f position;
    };

    Vec<RenderedSpan> renderedSpans;
    sf::Vector2f totalSize;
    bool dirty = true;

    void advanceLine(float* currentX, float* currentY, float* currentLineMaxHeight);
    sf::Text makeTextNode(const TextSpan& span, StringView t, u32 style);
    void appendText(
        const TextSpan& span, StringView t, u32 style,
        float* currentX, float* currentY, float* currentLineMaxHeight
    );
    void processSpan(
        const TextSpan& span,
        float* currentX, float* currentY, float* currentLineMaxHeight
    );

    void computeLayout();
};

} // namespace cx::ui

