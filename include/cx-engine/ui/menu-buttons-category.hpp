#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cx-engine/core/app.hpp>
#include <cx-engine/ui/dynamic-button-list.hpp>
#include <cx-engine/ui/menu-button.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>

namespace cx::ui {

class MenuButtonsCategory: public sf::Drawable {
public:
    enum class CategoryType {
        WithTitle,
        NoTitle
    };

private:
    App& app;
    const sf::Font& font;
    float uiScale;

    CategoryType type;
    UiScaleUnit titleSize;
    UiScaleUnit titleSpacing;
    UiScaleUnit listSpacing;
    sf::Color activeCategoryColor;
    sf::Color inactiveCategoryColor;

    Optional<sf::Text> title;

    static constexpr usize MaxButtons = 20;
    Buf<MenuButton, MaxButtons> buttons;

    DynamicButtonList buttonList;
    UiScaleUnit positionX;

public:
    MenuButtonsCategory(App& app, const sf::Font& font, const String& titleText, UiScaleUnit titleSize, CategoryType type = CategoryType::WithTitle);
    MenuButtonsCategory(MenuButtonsCategory&& other) noexcept;

    void addNewButton(const String& label, UiScaleUnit charSize, sf::Vector2f padding, const sf::Font& font, MenuButton::ClickCallback callback = nullptr);
    void addNewButton(const String& label, UiScaleUnit charSize, sf::Vector2f padding, MenuButton::ClickCallback callback = nullptr);

    template <typename... Args>
    void addButton(Args&&... args) {
        buttons.emplace(std::forward<Args>(args)...);
    }

    void resetButtons();

    float computeLayout(float y, float x, float width, float uiScale);
    void handle(const sf::Event& event);
    void gamepad(Gamepad& pad);
    void update(float dt);
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;

    void setHovered(bool hovered);
    void setSelectedIndex(Optional<usize> index);
    void setUiScale(float scale);
    void setTitleSpacing(float spacing);
    void setListSpacing(float spacing);
    void setActiveCategoryColor(sf::Color color);
    void setInactiveCategoryColor(sf::Color color);

    DynamicButtonList& getButtonList();
    sf::FloatRect getBounds() const;

    float getTitleSpacing() const { return titleSpacing; }
    float getListSpacing() const { return listSpacing; }
    sf::Color getActiveCategoryColor() const { return activeCategoryColor; }
    sf::Color getInactiveCategoryColor() const { return inactiveCategoryColor; }
    float getPositionX() const { return positionX; }
};

} // namespace cx
