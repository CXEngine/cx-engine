#include <cx-engine/defs/types.hpp>

#include <cx-engine/ui/menu-buttons-category.hpp>
#include <cx-engine/ui/menu-button.hpp>

#include <SFML/System/Vector2.hpp>

namespace cx::ui {

MenuButtonsCategory::MenuButtonsCategory(App& app, const sf::Font& font, const String& titleText, UiScaleUnit titleSize, CategoryType type)
    : app(app),
      font(font),
      uiScale(1.0f),
      type(type),
      titleSize(titleSize),
      titleSpacing(55.f),
      listSpacing(20.f),
      activeCategoryColor(sf::Color::White),
      inactiveCategoryColor(sf::Color(180, 180, 180)),
      positionX(0.f)
{
    if (type == CategoryType::WithTitle) {
        title.emplace(font, titleText, titleSize);
    }
}

MenuButtonsCategory::MenuButtonsCategory(MenuButtonsCategory&& other) noexcept
    : app(other.app),
      font(other.font),
      uiScale(other.uiScale),
      type(other.type),
      titleSize(other.titleSize),
      titleSpacing(other.titleSpacing),
      listSpacing(other.listSpacing),
      activeCategoryColor(other.activeCategoryColor),
      inactiveCategoryColor(other.inactiveCategoryColor),
      title(std::move(other.title)),
      buttons(std::move(other.buttons)),
      positionX(other.positionX)
{
    for (auto& button : buttons) {
        buttonList.addButton(button);
    }
}

void MenuButtonsCategory::addNewButton(const String& label, UiScaleUnit charSize, sf::Vector2f padding, const sf::Font& btnFont, MenuButton::ClickCallback callback) {
    auto& newButton = buttons.emplace(app, btnFont, label, charSize);
    newButton.setOnReleaseInside(callback);
    newButton.setPadding(padding);
    newButton.setUiScale(uiScale);
    buttonList.addButton(newButton);
}

void MenuButtonsCategory::addNewButton(const String& label, UiScaleUnit charSize, sf::Vector2f padding, MenuButton::ClickCallback callback) {
    return addNewButton(label, charSize, padding, font, callback);
}

void MenuButtonsCategory::resetButtons() {
    for(auto& btn: buttons) {
        btn.reset();
    }
}

float MenuButtonsCategory::computeLayout(float y, float x, float width, float currentUiScale) {
    uiScale = currentUiScale;
    UiScaleUnit currentY = y;
    positionX = x;

    if (title) {
        title->setCharacterSize(titleSize * uiScale);
        // idk what this 28.f stands for, but it just works so dont touch it
        UiScaleUnit magicNumber = 28.f;
        title->setPosition({ positionX - magicNumber * uiScale, currentY });
        currentY += title->getGlobalBounds().size.y + titleSpacing * uiScale;
    }

    if (buttons.isEmpty()) return currentY;

    for (auto& btn: buttons) {
        btn.setUiScale(uiScale);
    }

    buttonList.setSpacing(listSpacing * uiScale);
    float maxButtonHeight = 0;
    for (const auto& button : buttons) {
        if (button.getSize().y > maxButtonHeight) {
            maxButtonHeight = button.getSize().y;
        }
    }

    buttonList.setBounds({{positionX, currentY}, {width, maxButtonHeight }});

    currentY += buttonList.getBounds().size.y;
    return currentY;
}

void MenuButtonsCategory::handle(const sf::Event& event) {
    buttonList.handle(event);
}

void MenuButtonsCategory::gamepad(Gamepad& pad) {
    buttonList.gamepad(pad);
}

void MenuButtonsCategory::update(float dt) {
    for (auto& btn: buttons) {
        btn.setUiScale(uiScale);
    }
    buttonList.setUiScale(uiScale);
    buttonList.update(dt);
}

void MenuButtonsCategory::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (title) {
        target.draw(*title, states);
    }
    target.draw(buttonList, states);
}

void MenuButtonsCategory::setHovered(bool hovered) {
    if (title) {
        title->setFillColor(hovered ? activeCategoryColor : inactiveCategoryColor); // Use member colors
    }
}

void MenuButtonsCategory::setSelectedIndex(Optional<usize> index) {
    buttonList.setSelectedIndex(index);
}

void MenuButtonsCategory::setUiScale(float scale) {
    uiScale = scale;
    if (title) {
        title->setCharacterSize(titleSize * uiScale);
    }
    for (auto& btn : buttons) {
        btn.setUiScale(uiScale);
    }
}

void MenuButtonsCategory::setTitleSpacing(UiScaleUnit spacing) {
    titleSpacing = spacing;
}

void MenuButtonsCategory::setListSpacing(UiScaleUnit spacing) {
    listSpacing = spacing;
}

void MenuButtonsCategory::setActiveCategoryColor(sf::Color color) {
    activeCategoryColor = color;
}

void MenuButtonsCategory::setInactiveCategoryColor(sf::Color color) {
    inactiveCategoryColor = color;
}

DynamicButtonList& MenuButtonsCategory::getButtonList() {
    return buttonList;
}

sf::FloatRect MenuButtonsCategory::getBounds() const {
    sf::FloatRect listBounds = buttonList.getBounds();
    if (!title) {
        return listBounds;
    }
    sf::FloatRect titleBounds = title->getGlobalBounds();
    float top = titleBounds.position.y;
    float left = titleBounds.position.x;
    float width = std::max(titleBounds.size.x, listBounds.size.x);
    float height = (listBounds.position.y + listBounds.size.y) - top;
    return {{left, top}, {width, height}};
}

} // namespace cx
