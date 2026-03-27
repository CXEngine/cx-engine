#include <cx-engine/systems/settings/setting.hpp>
#include <cx-engine/ui/menu-button.hpp>

#include <cx-engine/ui/cut-corner-rect.hpp>
#include <cx-engine/core/app.hpp>

#include <algorithm>
#include <functional>

namespace cx::ui {

MenuButton::MenuButton(
    App& app,
    const sf::Font& font,
    std::string label,
    UiScaleUnit charSize
) : app(app),
    text(font, label, 0),
    storedCharSize(charSize)
{
    text.setFillColor(textIdleColor);
    rebuildBackground();
    rebuildPressEffect();
    setUiScale(uiScale);
}

sf::Color MenuButton::lerp(const sf::Color& a, const sf::Color& b, float t) {
    return {
        (byte) (a.r + (b.r - a.r) * t),
        (byte) (a.g + (b.g - a.g) * t),
        (byte) (a.b + (b.b - a.b) * t),
        (byte) (a.a + (b.a - a.a) * t)
    };
}

sf::Color MenuButton::lerpAlpha(const sf::Color& a, const sf::Color& b, float t) {
    return { a.r, a.g, a.b, (byte) (a.a + (b.a - a.a) * t) };
}

/*
void MenuButton::bindGamepadButton(PadButton button, sf::Texture& texture) {
    boundButton = button;
    buttonIconRawTexture = &texture;
    buttonIconSprite->setTexture(*buttonIconRawTexture, true);
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::bindGamepadIcon(PadButton button, const sf::Sprite& iconSprite) {
    boundButton = button;
    this->buttonIconSprite = iconSprite;
    buttonIconRawTexture = iconSprite.getTexture();
    rebuildBackground();
    rebuildPressEffect();
}
*/

void MenuButton::bindGamepadButton(PadButton button) {
    boundButton = button;
    updateIconSprite();
}

void MenuButton::updateIconSprite() {
    if (boundButton.has_value()) {
        if (app.gamepads.hasPrimary()) {
            buttonIconSprite = app.gamepadIcons.getIcon(app.gamepads, *boundButton);
        } else {
            buttonIconSprite = std::nullopt;
        }
    } else {
        buttonIconSprite = std::nullopt;
    }
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::unbindGamepadButton() {
    boundButton.reset();
    updateIconSprite();
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::rebuildBackground() {
    auto b = text.getLocalBounds();

    float iconWidth = 0.f;
    if (boundButton.has_value() && buttonIconSprite.has_value()) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
            float scale = textHeight / iconHeight;
            iconWidth = buttonIconSprite->getLocalBounds().size.x * scale + iconPadding * uiScale;
        }
    }

    const float width  = b.size.x + (padding.x * uiScale) * 2.f + iconWidth;
    const float height = b.size.y + (padding.y * uiScale) * 2.f;
    const float cut    = height * 0.35f;

    bg = createCutCornerRect(width, height, cut);
    bg.setFillColor(idleColor);
}

void MenuButton::rebuildPressEffect() {
    auto b = text.getLocalBounds();

    float iconWidth = 0.f;
    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
            float scale = textHeight / iconHeight;
            iconWidth = buttonIconSprite->getLocalBounds().size.x * scale + iconPadding * uiScale;
        }
    }

    const float width  = b.size.x + (padding.x * uiScale) * 2.f + iconWidth;
    const float height = b.size.y + (padding.y * uiScale) * 2.f;
    const float cut   = height * 0.35f;
    const float inset = 2.f * uiScale;

    pressEffect = createCutCornerRect(width, height, cut, inset);
    pressEffect.setFillColor({ 255, 255, 255, 0 });
}

void MenuButton::setPadding(sf::Vector2f pad) {
    padding = pad;
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::setHitboxPadding(sf::Vector2f hpad) {
    hitboxPadding = hpad;
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::setUiScale(float scale) {
    uiScale = scale;
    text.setCharacterSize(storedCharSize * uiScale);
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::setCharacterSize(UiScaleUnit charSize) {
    storedCharSize = charSize;
    text.setCharacterSize(storedCharSize * uiScale);
    rebuildBackground();
    rebuildPressEffect();
}

void MenuButton::setHovered(bool hovered) {
    externallyHovered = hovered;
}

void MenuButton::handle(const sf::Event& event) {
    if (auto mm = event.getIf<sf::Event::MouseMoved>()) {
        isHoveredInternal = getGlobalBounds().contains(sf::Vector2f(mm->position));
    }

    if (auto mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            if (isHoveredInternal || externallyHovered) {
                isPressed = true;
                press = 0.f;
            }
        }
    }

    if (auto mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) {
            if (isPressed) {
                isPressed = false;
                if (isHoveredInternal || externallyHovered) {
                    if (onReleaseInside) onReleaseInside(*this);
                    else if (onRelease) onRelease(*this);
                } else {
                    if (onRelease) onRelease(*this);
                }
            }
        }
    }
}

void MenuButton::gamepad(Gamepad& pad) {
    if (boundButton.has_value()) {
        bool currentlyPressed = pad.pressed(*boundButton);
        if (currentlyPressed && !isPressed) {
            isPressed = true;
            press = 0.f;
        } else if (!currentlyPressed && isPressed) {
            isPressed = false;
            if (onReleaseInside) onReleaseInside(*this);
            else if (onRelease) onRelease(*this);
        }
    }
}

void MenuButton::update(float dt) {
    updateIconSprite();
    wasPressedLastFrame = isPressed;

    const bool hovered = externallyHovered || isHoveredInternal;
    if (hovered) hover += dt * hoverSpeed;
    else hover -= dt * hoverSpeed;
    hover = std::clamp(hover, 0.f, 1.f);

    if (isPressed) {
        press = std::min(1.f, press + dt * pressSpeed);
    } else {
        press = std::max(0.f, press - dt * pressSpeed * 2.f);
    }

    float hoverT = hover * hover * (3.f - 2.f * hover);
    float pressT = press * press * (3.f - 2.f * press);

    auto b = text.getLocalBounds();

    float iconDisplayWidth = 0.f;
    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
            float scale = textHeight / iconHeight;
            iconDisplayWidth = buttonIconSprite->getLocalBounds().size.x * scale + iconPadding * uiScale;
        }
    }

    const float width  = b.size.x + (padding.x * uiScale) * 2.f + iconDisplayWidth;
    const float height = b.size.y + (padding.y * uiScale) * 2.f;

    sf::Vector2f pos = getPosition();
    sf::FloatRect bgRect = {
        { pos.x - width / 2.f, pos.y - height / 2.f },
        { width, height },
    };

    sf::Vector2f bgPos = { bgRect.position.x + (1.f - hoverT) * -(slide * uiScale), bgRect.position.y };
    bg.setScale({ hoverT, 1.f });
    bg.setPosition(bgPos);
    pressEffect.setScale({ hoverT, 1.f });
    pressEffect.setPosition(bgPos);

    sf::Vector2f bgCenter = { bgPos.x + width * 0.5f, bgPos.y + height * 0.5f };

    float effectSlide = hoverT * textSlide * uiScale + pressT * (1.f * uiScale);
    float totalContentWidth = b.size.x + iconDisplayWidth;
    float contentStartX = bgCenter.x - totalContentWidth / 2.f;

    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
             float scale = textHeight / iconHeight;
             buttonIconSprite->setScale({scale, scale});

             float iconCenterY = buttonIconSprite->getLocalBounds().size.y / 2.f;

             buttonIconSprite->setPosition({
                contentStartX + effectSlide - buttonIconSprite->getLocalBounds().position.x * scale,
                bgCenter.y - iconCenterY * scale + pressT * (1.f * uiScale)
             });
        }
    }

    sf::Vector2f glyphCenter = { b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f };
    text.setPosition({
        contentStartX + iconDisplayWidth - b.position.x + effectSlide,
        bgCenter.y - glyphCenter.y + pressT * (1.f * uiScale)
    });

    if (press > 0.f) {
        bg.setFillColor(lerp(hoverColor, pressColor, pressT));
        pressEffect.setFillColor(sf::Color(255,255,255,(byte)(255 * pressT)));
        text.setFillColor(lerp(textActiveColor, sf::Color::White, pressT * 0.5f));
    } else {
        bg.setFillColor(lerp(idleColor, hoverColor, hoverT));
        pressEffect.setFillColor(sf::Color(255,255,255,0));
        text.setFillColor(lerp(textIdleColor, textActiveColor, hoverT));
    }
}

sf::Vector2f MenuButton::getSize() const {
    auto b = text.getLocalBounds();
    float iconWidth = 0.f;
    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
            float scale = textHeight / iconHeight;
            iconWidth = buttonIconSprite->getLocalBounds().size.x * scale + iconPadding * uiScale;
        }
    }
    return { b.size.x + (padding.x * uiScale) * 2.f + iconWidth, b.size.y + (padding.y * uiScale) * 2.f };
}

sf::FloatRect MenuButton::getGlobalBounds() const {
    auto b = text.getLocalBounds();

    float iconDisplayWidth = 0.f;
    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        float textHeight = text.getCharacterSize();
        float iconHeight = buttonIconSprite->getLocalBounds().size.y;
        if (iconHeight > 0.f) {
            float scale = textHeight / iconHeight;
            iconDisplayWidth = buttonIconSprite->getLocalBounds().size.x * scale + iconPadding * uiScale;
        }
    }

    const float width  = b.size.x + (padding.x * uiScale) * 2.f + iconDisplayWidth;
    const float height = b.size.y + (padding.y * uiScale) * 2.f;

    sf::Vector2f pos = getPosition();
    sf::FloatRect bgRect = {
        { pos.x - width / 2.f, pos.y - height / 2.f },
        { width, height },
    };
    sf::FloatRect hitbox = bgRect;
    hitbox.position.x -= hitboxPadding.x * uiScale;
    hitbox.position.y -= hitboxPadding.y * uiScale;
    hitbox.size.x     += (hitboxPadding.x * uiScale) * 2.f;
    hitbox.size.y     += (hitboxPadding.y * uiScale) * 2.f;
    return hitbox;
}

bool MenuButton::wasClicked() const {
    return !wasPressedLastFrame && isPressed;
}

bool MenuButton::wasReleasedInside() const {
    Logger::debug("was released inside", {
        { "wasPressedLastFrame", toString(wasPressedLastFrame) },
    });

    return wasReleased() && (isHoveredInternal || externallyHovered);
}

bool MenuButton::wasReleased() const {
    return wasPressedLastFrame && !isPressed;
}

void MenuButton::triggerOnRelease() {
    if (onRelease) {
        onRelease(*this);
    }
}

sf::FloatRect MenuButton::getTextBounds() const {
    return text.getLocalBounds();
}

void MenuButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (hover > 0.f) {
        target.draw(bg, states);
        if (press > 0.f) target.draw(pressEffect, states);
    }

    if (boundButton.has_value() && buttonIconSprite.has_value() && buttonIconSprite->getTexture().getSize().x > 0) {
        sf::Color iconColor = text.getFillColor();
        buttonIconSprite->setColor(iconColor);
        target.draw(*buttonIconSprite, states);
    }

    target.draw(text, states);
}

} // namespace cx
