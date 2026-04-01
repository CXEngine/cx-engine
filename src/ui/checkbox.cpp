#include <cx-engine/ui/checkbox.hpp>

#include <cx-engine/systems/input/keyboard.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <algorithm>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx::ui {

sf::ConvexShape Checkbox::makeRoundedRect(sf::Vector2f size, float radius, unsigned int cornerPointCount) {
    sf::ConvexShape shape;
    unsigned int pointCount = cornerPointCount * 4;
    shape.setPointCount(pointCount);

    radius = std::min({radius, size.x / 2, size.y / 2});
    if (radius < 0) radius = 0;

    float angleStep = 90.f / (cornerPointCount > 1 ? cornerPointCount - 1 : 1);

    auto setCorner = [&](unsigned int startIndex, sf::Vector2f center, float startAngle) {
        for (unsigned int i = 0; i < cornerPointCount; ++i) {
            float angle = startAngle + i * angleStep;
            sf::Vector2f point(std::cos(angle * M_PI / 180.f) * radius, std::sin(angle * M_PI / 180.f) * radius);
            shape.setPoint(startIndex + i, center + point);
        }
    };

    setCorner(0, {size.x - radius, size.y - radius}, 0);
    setCorner(cornerPointCount, {radius, size.y - radius}, 90);
    setCorner(cornerPointCount * 2, {radius, radius}, 180);
    setCorner(cornerPointCount * 3, {size.x - radius, radius}, 270);

    return shape;
}

void Checkbox::addRectangleToShape(sf::VertexArray& shape, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4, const sf::Color& color) {
    shape.append(sf::Vertex(p1, color));
    shape.append(sf::Vertex(p2, color));
    shape.append(sf::Vertex(p3, color));

    shape.append(sf::Vertex(p1, color));
    shape.append(sf::Vertex(p3, color));
    shape.append(sf::Vertex(p4, color));
}

void Checkbox::addCircleToShape(sf::VertexArray& shape, sf::Vector2f center, float radius, int numSegments, const sf::Color& color) {
    for (int i = 0; i < numSegments; ++i) {
        float angle1 = (360.f / numSegments) * i * M_PI / 180.f;
        float angle2 = (360.f / numSegments) * (i + 1) * M_PI / 180.f;
        shape.append(sf::Vertex(center, color));
        shape.append(sf::Vertex(center + sf::Vector2f(std::cos(angle1), std::sin(angle1)) * radius, color));
        shape.append(sf::Vertex(center + sf::Vector2f(std::cos(angle2), std::sin(angle2)) * radius, color));
    }
}

void Checkbox::buildRoundedCheckmark(sf::VertexArray& shape, sf::Vector2f center, float size, float thickness, const sf::Color& color) {
    shape.clear();
    shape.setPrimitiveType(sf::PrimitiveType::Triangles);

    sf::Vector2f p1_base(-0.25f * size, 0.05f * size);
    sf::Vector2f p2_base(-0.05f * size, 0.3f * size);
    sf::Vector2f p3_base(0.3f * size, -0.2f * size);

    sf::Vector2f p1 = center + p1_base;
    sf::Vector2f p2 = center + p2_base;
    sf::Vector2f p3 = center + p3_base;

    sf::Vector2f dir1 = p2 - p1;
    float len1 = std::sqrt(dir1.x * dir1.x + dir1.y * dir1.y);
    sf::Vector2f unitDir1 = dir1 / len1;
    sf::Vector2f normal1(-unitDir1.y, unitDir1.x);

    sf::Vector2f dir2 = p3 - p2;
    float len2 = std::sqrt(dir2.x * dir2.x + dir2.y * dir2.y);
    sf::Vector2f unitDir2 = dir2 / len2;
    sf::Vector2f normal2(-unitDir2.y, unitDir2.x);

    sf::Vector2f rect1_p1 = p1 - normal1 * (thickness / 2.f);
    sf::Vector2f rect1_p2 = p1 + normal1 * (thickness / 2.f);
    sf::Vector2f rect1_p3 = p2 + normal1 * (thickness / 2.f);
    sf::Vector2f rect1_p4 = p2 - normal1 * (thickness / 2.f);
    addRectangleToShape(shape, rect1_p1, rect1_p2, rect1_p3, rect1_p4, color);

    sf::Vector2f rect2_p1 = p2 - normal2 * (thickness / 2.f);
    sf::Vector2f rect2_p2 = p2 + normal2 * (thickness / 2.f);
    sf::Vector2f rect2_p3 = p3 + normal2 * (thickness / 2.f);
    sf::Vector2f rect2_p4 = p3 - normal2 * (thickness / 2.f);
    addRectangleToShape(shape, rect2_p1, rect2_p2, rect2_p3, rect2_p4, color);

    addCircleToShape(shape, p1, thickness / 2.f, 16, color);
    addCircleToShape(shape, p2, thickness / 2.f, 16, color);
    addCircleToShape(shape, p3, thickness / 2.f, 16, color);
}


Checkbox::Checkbox() {
    checkmarkShape.setPrimitiveType(sf::PrimitiveType::Triangles);
    setUiScale(1.f);
    setColor({255,255,255,255});
}

void Checkbox::setUiScale(float s) {
    uiScale = s;
    boxSize = 32.f * uiScale;
    float cornerRadius = 8.f * uiScale;
    float outline = 2.f * uiScale;

    box = makeRoundedRect({boxSize, boxSize}, cornerRadius, 24);
    box.setOutlineThickness(outline);

    updateLayout();
}

void Checkbox::setColor(sf::Color clr) {
    color = clr;
    box.setOutlineColor(clr);

    if (clr.r * 0.299 + clr.g * 0.587 + clr.b * 0.114 > 150) {
        checkmarkColor = sf::Color(20, 20, 20);
    } else {
        checkmarkColor = sf::Color(230, 230, 230);
    }
    updateLayout();
}

void Checkbox::setChecked(bool c, bool animate) {
    checked = c;
    if (!animate) {
        animationProgress = checked ? 1.f : 0.f;
    }
    updateLayout();
}

bool Checkbox::isChecked() const {
    return checked;
}

void Checkbox::setPosition(const sf::Vector2f& pos) {
    position = pos;
    updateLayout();
}

sf::FloatRect Checkbox::getGlobalBounds() const {
    return box.getGlobalBounds();
}

sf::Vector2f Checkbox::getSize() const {
    return { boxSize, boxSize };
}

void Checkbox::handle(const sf::Event& event) {
    if (hasFlag(inputMode, InputMode::Mouse)) {
        if (auto mp = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mp->button != sf::Mouse::Button::Left) return;
            if (getGlobalBounds().contains(sf::Vector2f(mp->position))) {
                setChecked(!checked, true);
            }
        }
    }

    if (hasFlag(inputMode, InputMode::Keyboard) && hasFocus()) {
        if (auto kp = event.getIf<sf::Event::KeyReleased>()) {
            if (kp->code != cx::Key::Enter) return;
            setChecked(!checked, true);
        }
    }
}

void Checkbox::gamepad(Gamepad& gamepad) {
    if (hasFlag(inputMode, InputMode::Gamepad) && hasFocus()) {
        if (gamepad.justPressed(PadButton::South)) {
            setChecked(!checked, true);
        }
    }
}

void Checkbox::update(float dt) {
    const float animationSpeed = 5.f;
    if (checked && animationProgress < 1.f) {
        animationProgress = std::min(1.f, animationProgress + dt * animationSpeed);
        updateLayout();
    } else if (!checked && animationProgress > 0.f) {
        animationProgress = std::max(0.f, animationProgress - dt * animationSpeed);
        updateLayout();
    }
}

void Checkbox::updateLayout() {
    box.setPosition(position);

    std::uint8_t alpha = 100 + (255 - 100) * animationProgress;
    box.setFillColor(sf::Color(color.r, color.g, color.b, alpha));

    sf::Vector2f checkmarkCenter = position + sf::Vector2f(boxSize / 2.f, boxSize / 2.f);
    float checkmarkThickness = 4.f * uiScale;
    float checkmarkSize = boxSize * 0.6f;

    buildRoundedCheckmark(checkmarkShape, checkmarkCenter, checkmarkSize, checkmarkThickness, checkmarkColor);
}

void Checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(box, states);

    if (animationProgress > 0.f) {
        sf::Transform t;
        t.translate(box.getPosition() + sf::Vector2f(boxSize / 2.f, boxSize / 2.f));
        t.scale(sf::Vector2f(animationProgress, animationProgress));
        t.translate(-(box.getPosition() + sf::Vector2f(boxSize / 2.f, boxSize / 2.f)));

        sf::RenderStates checkStates = states;
        checkStates.transform *= t;

        target.draw(checkmarkShape, checkStates);
    }
}

} // namespace cx
