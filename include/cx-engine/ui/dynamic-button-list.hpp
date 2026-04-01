#pragma once

#include <cx-engine/ui/widget.hpp>
#include <cx-engine/defs/types.hpp>

#include <cx-engine/ui/menu-button.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>

#include <SFML/Graphics.hpp>

namespace cx::ui {

class DynamicButtonList: public Widget {
private:
    Vec<MenuButton*> buttons;
    Vec<float> animWidths;

    bool gamepadConfirmHeld;

    Optional<usize> selectedIndex = std::nullopt;
    Optional<usize> hoveredIndex = std::nullopt;

    UiScaleUnit spacing;
    sf::FloatRect bounds;
    bool navigationEnabled = true;
    float uiScale = 1.f;

public:
    DynamicButtonList();

    void addButton(MenuButton& button);
    void setBounds(const sf::FloatRect& rect);
    void setSpacing(UiScaleUnit newSpacing);
    void setNavigationEnabled(bool enabled);
    void setUiScale(float scale) override;

    void clearButtons() {
        buttons.clear();
        animWidths.clear();
        selectedIndex = std::nullopt;
    }

    sf::Vector2f getSize() const override {
        return getGlobalBounds().size;
    }

    void update(float dt) override;
    void handle(const sf::Event& event) override;
    void gamepad(Gamepad& gamepad) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    bool justReleased(usize index);

    Slice<MenuButton*> getButtons() { return Slice(buttons); }
    Slice<const MenuButton* const> getButtons() const { return Slice(buttons); };
    usize getButtonCount() const { return buttons.size(); }

    Optional<usize> getSelectedIndex() const { return selectedIndex; }
    void setSelectedIndex(Optional<usize> index) { selectedIndex = index; }

    sf::FloatRect getBounds() const { return bounds; }
    float getSpacing() const { return spacing; }
};

} // namespace cx
