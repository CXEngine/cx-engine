#pragma once

#include <cx-engine/ui/widget.hpp>
#include <cx-engine/systems/input/mouse.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace cx::ui {

class Checkbox: public Widget {
public:
    Checkbox();

    void setUiScale(float s) override;
    void setColor(sf::Color clr);
    void setChecked(bool checked, bool animate = true);
    bool isChecked() const;
    void setPosition(const sf::Vector2f& pos);

    sf::FloatRect getGlobalBounds() const override;
    sf::Vector2f getSize() const override;

    void gamepad(Gamepad& gamepad) override;
    void handle(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void updateLayout();

    static sf::ConvexShape makeRoundedRect(sf::Vector2f size, float radius, unsigned int cornerPointCount);
    static void buildRoundedCheckmark(sf::VertexArray& shape, sf::Vector2f center, float size, float thickness, const sf::Color& color);
    static void addRectangleToShape(sf::VertexArray& shape, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4, const sf::Color& color);
    static void addCircleToShape(sf::VertexArray& shape, sf::Vector2f center, float radius, int numSegments, const sf::Color& color);

    sf::ConvexShape box;
    sf::VertexArray checkmarkShape;
    sf::Color checkmarkColor;
    sf::Color color;
    float uiScale = 1.f;

    sf::Vector2f position;

    float boxSize = 28.f;
    bool checked = false;

    float animationProgress = 0.f; // 0.0 = unchecked, 1.0 = checked
};

} // namespace cx::ui
