#include <cx-engine/ui/dialog-box.hpp>
#include <cx-engine/ui/cut-corner-rect.hpp>

namespace cx::ui {

void DialogBox::centerElements() {
    sf::Vector2f viewSize = app.win.getView().getSize();
    float dialogWidth = viewSize.x * 0.6f;
    float dialogHeight = viewSize.y * 0.4f;

    background = createCutCornerRect(dialogWidth, dialogHeight, 20.f * app.uiScale);
    background.setFillColor(fillColor);
    background.setOutlineColor(borderColor);
    background.setOutlineThickness(borderWidth);
    background.setOrigin({ background.getLocalBounds().size.x / 2.f, background.getLocalBounds().size.y / 2.f });
    background.setPosition({ viewSize.x / 2.f, viewSize.y / 2.f });
}

void DialogBox::updateLayout() {
    centerElements();

    sf::Vector2f dialogPos = background.getPosition();
    sf::FloatRect bgBounds = background.getLocalBounds();
    sf::Vector2f dialogTopLeft = { dialogPos.x - bgBounds.size.x / 2.f, dialogPos.y - bgBounds.size.y / 2.f };

    float padding = 20.f * app.uiScale;
    float currentY = dialogTopLeft.y + padding;

    // Title layout
    titleBrowser.setUiScale(app.uiScale);
    titleBrowser.setMaxWidth(bgBounds.size.x * 0.9f);
    titleBrowser.update(0.f);
    sf::Vector2f titleSize = titleBrowser.getSize();
    titleBrowser.setPosition({ dialogTopLeft.x + (bgBounds.size.x - titleSize.x) / 2.f, currentY });
    currentY += titleSize.y + padding;

    // Description layout
    descriptionBrowser.setUiScale(app.uiScale);
    descriptionBrowser.setMaxWidth(bgBounds.size.x * 0.8f);
    descriptionBrowser.update(0.f);
    sf::Vector2f descSize = descriptionBrowser.getSize();
    descriptionBrowser.setPosition({ dialogTopLeft.x + (bgBounds.size.x - descSize.x) / 2.f, currentY });
    currentY += descSize.y + padding;

    float totalButtonsWidth = 0;
    if (!buttons.empty()) {
        for (const auto& btnPtr: buttons) {
            totalButtonsWidth += btnPtr->getSize().x;
        }
        totalButtonsWidth += (buttons.size() - 1) * buttonList.getSpacing();
    }

    float buttonListX = dialogTopLeft.x + (bgBounds.size.x - totalButtonsWidth) / 2.f;

    sf::FloatRect buttonListBounds = sf::FloatRect {
        { buttonListX, dialogTopLeft.y + bgBounds.size.y - (60.f * app.uiScale) - padding },
        { totalButtonsWidth, 60.f * app.uiScale },
    };
    buttonList.setBounds(buttonListBounds);

    if (closeButton && allowClose) {
        sf::Vector2f closeBtnSize = closeButton->getSize();
        closeButton->setPosition({
            dialogTopLeft.x + bgBounds.size.x - closeBtnSize.x / 2.f - padding / 2.f,
            dialogTopLeft.y + closeBtnSize.y / 2.f + padding / 2.f
        });
    }
}

DialogBox::DialogBox(App& app, const sf::Font& font, TextDocument title, TextDocument description, InitList<ButtonConfig> btns,
                     sf::Color borderColor, float borderWidth)
    : app(app)
    , closeButton(MenuButton(app, font, "X", u32(20.f * app.uiScale)))
    , fillColor(sf::Color(0, 0, 0, 180))
    , borderColor(borderColor)
    , borderWidth(borderWidth)
{
    titleBrowser.setDocument(std::move(title));
    descriptionBrowser.setDocument(std::move(description));
    setButtons(btns);

    closeButton->setPadding({12.f * app.uiScale, 6.f * app.uiScale});
    closeButton->setSlideAmount(0.f);
    closeButton->setIdleColor({  });
    closeButton->setHoverColor({ 255, 80, 80, 255 });
    closeButton->setTextIdleColor(sf::Color::White);
    closeButton->setTextActiveColor(sf::Color::White);
    closeButton->setOnReleaseInside([this](MenuButton&) {
        this->close();
    });

    updateLayout();
}

void DialogBox::setButtons(InitList<ButtonConfig> btns) {
    buttonList.clearButtons();
    buttons.clear();

    u32 charSize = u32(24.f * app.uiScale);
    if (!buttons.empty()) {
        charSize = buttons[0]->getTextBounds().size.y;
    }

    for (const auto& config: btns) {
        // use default closeButton font as fallback for buttons
        auto btn = std::make_unique<MenuButton>(app, closeButton->getFont(), config.label, charSize);
        btn->setUiScale(app.uiScale);
        btn->setPadding({30.f * app.uiScale, 15.f * app.uiScale});
        btn->setOnRelease([this, id = config.id](MenuButton&) {
            this->pressedButtonId = id;
            this->close();
        });
        if (config.gamepadButton) {
            btn->bindGamepadButton(*config.gamepadButton);
        }
        buttonList.addButton(*btn);
        buttons.push_back(std::move(btn));
    }
    buttonList.setSpacing(10.f * app.uiScale);
    updateLayout();
}

void DialogBox::setTitle(TextDocument newTitle) {
    titleBrowser.setDocument(std::move(newTitle));
    updateLayout();
}

void DialogBox::setDescription(TextDocument newDescription) {
    descriptionBrowser.setDocument(std::move(newDescription));
    updateLayout();
}

void DialogBox::setButtonFont(const sf::Font& font) {
    for (auto& btn: buttons) {
        btn->setFont(font);
    }
    closeButton->setFont(font);
    updateLayout();
}

void DialogBox::setBackgroundColor(sf::Color color) {
    fillColor = color;
    updateLayout();
}

void DialogBox::setShowCloseButton(bool show) {
    allowClose = show;
}

void DialogBox::setOnClose(CloseCallback callback) {
    onClose = callback;
}

void DialogBox::open() {
    isopen = true;
    pressedButtonId.reset();
    buttonList.setSelectedIndex(buttonList.getButtonCount() > 0 ? 0 : -1);
    updateLayout();
}

void DialogBox::close() {
    if (!isOpen()) return;
    isopen = false;
    if (onClose) {
        onClose(*this);
    }
}

Optional<String> DialogBox::getPressedButtonId() const {
    return pressedButtonId;
}

void DialogBox::update(float dt) {
    if (!isOpen()) return;

    buttonList.update(dt);
    titleBrowser.update(dt);
    descriptionBrowser.update(dt);

    cx::Gamepad* pad = app.gamepads.hasPrimary() ? &app.gamepads.get() : nullptr;
    if (pad) {
        buttonList.gamepad(*pad);
    }

    if (closeButton && allowClose) {
        if (pad) {
            closeButton->gamepad(*pad);
        }
        closeButton->update(dt);
    }
}

void DialogBox::handle(const sf::Event& event) {
    if (!isOpen()) return;

    if (allowClose) {
        if (auto kp = event.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                close();
            }
        }
    }
}

void DialogBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!isOpen()) return;

    target.draw(background, states);
    target.draw(titleBrowser, states);
    target.draw(descriptionBrowser, states);
    target.draw(buttonList, states);
    if (closeButton && allowClose) {
        target.draw(*closeButton, states);
    }
}

}
