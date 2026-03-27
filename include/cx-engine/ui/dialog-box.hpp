#pragma once

#include <SFML/Graphics.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/core/app.hpp>
#include <cx-engine/ui/dynamic-button-list.hpp>
#include <cx-engine/ui/menu-button.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>

namespace cx::ui {

class DialogBox: public sf::Drawable {
public:
    struct ButtonConfig {
        String id;
        String label;
        Optional<PadButton> gamepadButton;
    };

    using CloseCallback = Func<void(DialogBox&)>;

private:
    App& app;
    sf::ConvexShape background;

    sf::Text titleText;
    sf::Text descriptionText;

    DynamicButtonList buttonList;
    Vec<UniquePtr<MenuButton>> buttons;

    Optional<MenuButton> closeButton;
    bool allowClose = true;

    bool isopen = false;
    Optional<String> pressedButtonId;

    CloseCallback onClose;

    sf::Color fillColor;
    sf::Color borderColor;
    float borderWidth;

    void updateLayout();
    void centerElements();

public:
    DialogBox(App& app, const sf::Font& font, String title, String description, InitList<ButtonConfig> btns,
              sf::Color borderColor = sf::Color::White, float borderWidth = 2.f);

    void setButtons(InitList<ButtonConfig> btns);

    void setTitle(String newTitle);
    void setDescription(String newDescription);

    void setTitleFont(const sf::Font& font);
    void setDescriptionFont(const sf::Font& font);
    void setButtonFont(const sf::Font& font);

    void setTitleCharacterSize(u32 size);
    void setDescriptionCharacterSize(u32 size);

    void setTitleColor(sf::Color color);
    void setDescriptionColor(sf::Color color);
    void setBackgroundColor(sf::Color color);
    void setOverlayColor(sf::Color color);

    void setShowCloseButton(bool show);
    void setOnClose(CloseCallback callback);

    void bindCloseButton(PadButton btn) {
        if (closeButton)
            closeButton->bindGamepadButton(btn);
    }

    void open();
    void close();

    bool isOpen() const { return isopen; }
    Optional<String> getPressedButtonId() const;

    void update(float dt);
    void handle(const sf::Event& event);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
