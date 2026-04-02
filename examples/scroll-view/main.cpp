#include <cx-engine/core/app.hpp>

#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/app/config-drivers/default.hpp>

#include <cx-engine/ui/text-browser.hpp>
#include <cx-engine/ui/text/builder.hpp>
#include <cx-engine/ui/scroll-view.hpp>
#include <cx-engine/ui/widget.hpp>
#include <cx-engine/ui/checkbox.hpp>
#include <cx-engine/ui/menu-button.hpp>
#include <cx-engine/utils/strconv.hpp>

#include <ctime>

class TestCompositeWidget: public cx::ui::Widget {
public:
    static constexpr cx::usize NumCheckboxes = 8;
    static constexpr cx::usize NumButtons = 8;

    cx::ui::TextBrowser textBrowser;
    cx::Buf<cx::ui::Checkbox, NumCheckboxes> checkboxes;
    cx::Buf<cx::ui::MenuButton, NumButtons>  buttons;
    cx::Buf<cx::ui::TextBrowser, NumButtons> texts;

    TestCompositeWidget() = default;

    TestCompositeWidget(cx::App& app, const sf::Font& font, const cx::ui::TextDocument& doc) {
        init(app, font, doc);
    }

    void init(cx::App& app, const sf::Font& font, const cx::ui::TextDocument& doc) {
        textBrowser.setDocument(doc);
        textBrowser.setWrapMode(cx::ui::TextWrap::NoWrap);

        checkboxes.clear();
        for (int i = 0; i < 8; ++i) {
            cx::ui::Checkbox cb;
            cb.setInputMode(cx::ui::InputMode::Mouse);
            cb.setColor({ (cx::u8) (100 + i * 20), (cx::u8) (200 - i * 15), 250, 255 });
            checkboxes.push(std::move(cb));
        }

        texts.clear();
        for (cx::uint i = 0; i < 5; ++i) {
            cx::ui::TextBrowser text;
            cx::ui::TextDocument doc = cx::ui::TextDocumentBuilder({ .font = app.defaultFont, .size = 20 })
                .setColor(sf::Color::Cyan)
                .setBold(true)
                .addPart("Clicked!")
                .build();

            text.setDocument(std::move(doc));
            text.setPosition({-1000.f, -1000.f});
            texts.push(std::move(text));
        }

        buttons.clear();
        for (cx::uint i = 0; i < 5; ++i) {
            cx::ui::MenuButton btn(app, font, "Scroll Test Button " + cx::toString(i), 30);
            btn.setInputMode(cx::ui::InputMode::Mouse);
            btn.setOnRelease([this, i](cx::ui::MenuButton& b) {
                sf::Vector2f btnPos = b.getPosition();
                float btnHalfWidth = b.getSize().x / 2.f;
                texts[i].setPosition({ btnPos.x + btnHalfWidth + 30.f, btnPos.y - 15.f });
                cx::Logger::info("Button " + cx::toString(i) + " clicked!");
            });
            buttons.push(std::move(btn));
        }

        updateLayout();
    }

    void updateLayout() {
        float y = 30.f;
        
        textBrowser.setPosition({ 30.f, y });
        textBrowser.update(0.f);
        y += textBrowser.getSize().y + 50.f;

        for (auto& cb : checkboxes) {
            cb.setPosition({ 50.f, y });
            y += cb.getSize().y + 25.f;
        }

        y += 30.f;

        for (auto& btn : buttons) {
            btn.setPosition({ 350.f, y });
            y += btn.getSize().y + 20.f;
        }

        size = { 1200.f, y + 50.f };
    }

    sf::Vector2f size = { 0.f, 0.f };
    sf::Vector2f getSize() const override { return size; }
    
    void setUiScale(float scale) override {
        textBrowser.setUiScale(scale);
        for (auto& cb: checkboxes) cb.setUiScale(scale);
        for (auto& btn: buttons)   btn.setUiScale(scale);
        for (auto& txt: texts)     txt.setUiScale(scale);
        updateLayout();
    }

    void handle(const sf::Event& event) override {
        textBrowser.handle(event);
        for (auto& cb:  checkboxes) cb.handle(event);
        for (auto& btn: buttons)    btn.handle(event);
        for (auto& txt: texts)      txt.handle(event);
    }

    void update(float dt) override {
        textBrowser.update(dt);
        for (auto& cb: checkboxes) cb.update(dt);
        for (auto& btn: buttons)   btn.update(dt);
        for (auto& txt: texts)     txt.update(dt);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(textBrowser, states);
        for (auto& cb: checkboxes) target.draw(cb, states);
        for (auto& btn: buttons)   target.draw(btn, states);
        for (auto& txt: texts)     target.draw(txt, states);
    }
};

cx::String getRandomSentence() {
    cx::String sentences[] = {
        "Have a nice day!",
        "It works on my machine.",
        "This is probably fine.",
        "I have no idea what I'm doing.",
        "There is no bug, only features.",
        "Something is definitely not right.",
        "Trust me, I'm an engineer.",
        "This seemed like a good idea at the time.",
        "Error? What error?",
    };
    return sentences[rand() % std::size(sentences)];
}

cx::u8 randColorChan() {
    return (cx::u8) (rand() % 256);
}

cx::ui::TextDocument makeDocument(const sf::Font& font, cx::uint howMany) {
    cx::ui::TextDocumentBuilder b({ 
        .font = font, 
        .size = 36, 
        .color = sf::Color::White,
        .bold = false,
        .italic = false
    });
    b.addPart("Testing ScrollView with various nested widgets.\n\n");
    for (cx::uint i = 0; i < howMany; ++i) {
        b.setColor({ randColorChan(), randColorChan(), randColorChan(), 255 });
        b.addPart("Line #" + cx::toString(i) + ": ");
        b.addPart(getRandomSentence());
        b.addPart("\n");
    }

    return std::move(b).build();
}

class Game: public cx::App {
public:
    cx::DefaultConfigDriver config;
    cx::ui::ScrollView<TestCompositeWidget> scrollView;

    Game() : cx::App("Scroll view example") {
        forceRecreateWindow(cx::WindowType::Normal);

        srand((unsigned int)time(NULL));
        cx::ui::TextDocument doc = makeDocument(defaultFont, 15);

        scrollView.getContent().init(*this, defaultFont, doc);

        scrollView.smoothScrollSpeed = sf::Vector2f(25.f, 25.f);
        scrollView.mouseScrollSpeed = sf::Vector2f(120.f, 120.f);
        scrollView.setInputMode(cx::ui::InputMode::All);
        scrollView.setSize(sf::Vector2f(config.getTargetResolution()));

        cx::Logger::info("Improved ScrollView example with nested widgets loaded.");
    }

    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void update(float dt) override {
        cx::App::update(dt);
        scrollView.update(dt);
        for (auto& gamepad: gamepads) {
            scrollView.gamepad(gamepad);
        }
    }

    void handle(const sf::Event& event) override {
        cx::App::handle(event);
        scrollView.handle(event);
    }

    void drawGame(sf::RenderTarget& target) override {}

    void drawUi(sf::RenderTarget& target) override {
        target.draw(scrollView);
    }
};

int main() {
    Game game;
    return game.run();
}
