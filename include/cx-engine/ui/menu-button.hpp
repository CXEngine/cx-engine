#pragma once

#include <cx-engine/core/app.hpp>
#include <cx-engine/ui/widget.hpp>

#include <cx-engine/defs/types.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>
#include <cx-engine/systems/gamepad/gamepad-manager.hpp>
#include <cx-engine/systems/gamepad/gamepad-icons.hpp>

namespace cx::ui {

class MenuButton: public Widget {
public:
    using ClickCallback = Func<void(MenuButton&)>;

private:
    App& app;

    sf::Text text;
    sf::ConvexShape bg;
    sf::ConvexShape pressEffect;

    float hover = 0.f;
    float press = 0.f;
    const float hoverSpeed = 6.f;
    const float pressSpeed = 15.f;

    sf::Vector2f padding       = { 48.f, 20.f };
    sf::Vector2f hitboxPadding = { 20.f, 12.f };

    UiScaleUnit slide = 40.f;
    UiScaleUnit textSlide = 4.f;
    float uiScale = 1.f;

    bool isPressed = false;
    bool wasPressedLastFrame = false;
    bool isHoveredInternal = false;
    bool externallyHovered = false;

    ClickCallback onClick = nullptr;
    ClickCallback onRelease = nullptr;
    ClickCallback onReleaseInside = nullptr;

    sf::Color idleColor        = { 255, 255, 255, 255 };
    sf::Color hoverColor       = { 200, 200, 200, 255 };
    sf::Color pressColor       = { 180, 180, 255, 255 };
    sf::Color textIdleColor    = { 255, 255, 255, 255 };
    sf::Color textActiveColor  = { 30,  30,  40,  255 };

    Optional<PadButton> boundButton;
    mutable Optional<sf::Sprite> buttonIconSprite;
    UiScaleUnit iconPadding = 10.f;
    UiScaleUnit storedCharSize;

    static sf::Color lerp(const sf::Color& a, const sf::Color& b, float t);
    static sf::Color lerpAlpha(const sf::Color& a, const sf::Color& b, float t);
    void rebuildBackground();
    void rebuildPressEffect();
    void updateIconSprite();

public:
    MenuButton(App& app, const sf::Font& font, String label, UiScaleUnit charSize);

    void setFont(const sf::Font& font) { text.setFont(font); }
    const sf::Font& getFont() const { return text.getFont(); }

    void reset() {
        isPressed = false;
        wasPressedLastFrame = false;
    }

    void setPadding(sf::Vector2f pad);
    void setHitboxPadding(sf::Vector2f hpad);
    void setUiScale(float scale) override;
    void setCharacterSize(UiScaleUnit charSize);
    void setHovered(bool hovered);

    void bindGamepadButton(PadButton button);
    void unbindGamepadButton();

    void handle(const sf::Event& event) override;
    void gamepad(Gamepad& gamepad) override;
    void update(float dt) override;

    void setOnClick(ClickCallback callback) { onClick = callback; }
    void setOnRelease(ClickCallback callback) { onRelease = callback; }
    void setOnReleaseInside(ClickCallback callback) { onReleaseInside = callback; }

    bool isHovered() const { return hover > 0.1f; }
    bool isButtonPressed() const { return isPressed; }
    sf::Vector2f getSize() const override;
    sf::FloatRect getGlobalBounds() const override;

    bool wasClicked() const;
    bool wasReleasedInside() const;
    bool wasReleased() const;
    void triggerOnRelease();

    void setIdleColor(sf::Color color) { idleColor = color; }
    void setHoverColor(sf::Color color) { hoverColor = color; }
    void setPressColor(sf::Color color) { pressColor = color; }
    void setTextIdleColor(sf::Color color) { textIdleColor = color; }
    void setTextActiveColor(sf::Color color) { textActiveColor = color; }

    void setSlideAmount(float amount) { slide = amount; }
    void setTextSlideAmount(float amount) { textSlide = amount; }

    String getText() const { return text.getString().toAnsiString(); }
    sf::FloatRect getTextBounds() const;

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;
};

}
