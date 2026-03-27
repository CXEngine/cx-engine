#include <cx-engine/systems/input/mouse.hpp>
#include <cx-engine/ui/dynamic-button-list.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>

namespace cx::ui {

DynamicButtonList::DynamicButtonList() : spacing(20.f), selectedIndex(std::nullopt) {}

void DynamicButtonList::addButton(MenuButton& button) {
    buttons.push_back(&button);
    animWidths.push_back(button.getSize().x);
}

void DynamicButtonList::setBounds(const sf::FloatRect& rect) {
    bounds = rect;
}

void DynamicButtonList::setSpacing(UiScaleUnit newSpacing) {
    spacing = newSpacing;
}

void DynamicButtonList::setNavigationEnabled(bool enabled) {
    navigationEnabled = enabled;
    if (!navigationEnabled) {
        selectedIndex = std::nullopt;
        for (auto& button : buttons) {
            button->setHovered(false);
        }
    } else {
        if (!buttons.empty() && !selectedIndex.has_value()) {
            selectedIndex = 0U;
        }
    }
}

void DynamicButtonList::setUiScale(float scale) {
    uiScale = scale;
}

void DynamicButtonList::gamepad(Gamepad& pad) {
    if (!navigationEnabled || !hasFlag(inputMode, UiInputMode::Gamepad) || buttons.empty()) {
        selectedIndex = std::nullopt;
        return;
    }

    gamepadConfirmHeld = pad.pressed(PadButton::South);

    if (!selectedIndex.has_value()) {
        selectedIndex = 0U;
    }

    if (pad.justScrolledRight()) {
        *selectedIndex = (*selectedIndex + 1) % buttons.size();
    } else if (pad.justScrolledLeft()) {
        *selectedIndex = (*selectedIndex == 0)
            ? buttons.size() - 1
            : *selectedIndex - 1;
    }

    for (auto* button : buttons) {
        button->gamepad(pad);
    }
}

void DynamicButtonList::handle(const sf::Event& event) {
    for (auto* button : buttons) {
        button->handle(event);
    }
}

void DynamicButtonList::update(float dt) {
    if (buttons.empty()) {
        selectedIndex = std::nullopt;
        return;
    }

    const UiScaleUnit hoverExpansion = 30.f;
    const float animationSpeed = 8.f;

    hoveredIndex = std::nullopt;

    for (usize i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->getGlobalBounds().contains(sf::Vector2f(cx::Mouse::getCursorPos()))) {
            hoveredIndex = i;
            break;
        }
    }

    if (!hoveredIndex.has_value()) {
        hoveredIndex = selectedIndex;
    }

    for (usize i = 0; i < buttons.size(); ++i) {
        MenuButton* button = buttons[i];

        bool hovered = hoveredIndex.has_value() && *hoveredIndex == i;
        button->setHovered(hovered);

        button->update(dt);

        float baseWidth = button->getSize().x;
        float targetWidth = hovered
            ? baseWidth + hoverExpansion * uiScale
            : baseWidth;

        animWidths[i] += (targetWidth - animWidths[i]) * dt * animationSpeed;
    }

    float currentX = bounds.position.x;

    float maxTextHeight = 0.f;
    float maxTextCenterY = 0.f;
    for (const auto& button : buttons) {
        auto t = button->getTextBounds();
        if (t.size.y > maxTextHeight) {
            maxTextHeight = t.size.y;
            maxTextCenterY = t.position.y + t.size.y * 0.5f;
        }
    }

    float maxButtonHeight = 0.f;
    for (const auto& button : buttons) {
        maxButtonHeight = std::max(maxButtonHeight, button->getSize().y);
    }

    const UiScaleUnit bottomMargin = 20.f;
    float baseY = bounds.position.y + bounds.size.y
        - maxButtonHeight - bottomMargin * uiScale;

    float refTextCenter = baseY + maxTextCenterY;

    for (usize i = 0; i < buttons.size(); ++i) {
        MenuButton* button = buttons[i];
        auto t = button->getTextBounds();

        float textCenter = t.position.y + t.size.y * 0.5f;
        float y = refTextCenter - textCenter;

        button->setPosition({
            currentX + button->getSize().x * 0.5f,
            y
        });

        currentX += animWidths[i] + spacing * uiScale;
    }
}

void DynamicButtonList::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& button : buttons) {
        target.draw(*button, states);
    }
}

bool DynamicButtonList::justReleased(usize index) {
    if (index >= buttons.size()) return false;
    return buttons[index]->wasReleased();
}

} // namespace cx
