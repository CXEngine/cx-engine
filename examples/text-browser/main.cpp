#include <cx-engine/core/app.hpp>

#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/app/config-drivers/default.hpp>

#include <cx-engine/ui/text-browser.hpp>
#include <cx-engine/ui/text/builder.hpp>
#include <cx-engine/ui/ui-widget.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

cx::String repeat(cx::StringView s, cx::usize times) {
    cx::String result;
    result.reserve(s.length() * times);
    for (cx::usize i = 0; i < times; ++i) {
        result += s;
    }
    return result;
}

class Game: public cx::App {
public:
    cx::ui::TextBrowser browser;
    sf::Font quicksandFont;

    Game() : cx::App("Text browser example") {}

    void init() override {
        forceRecreateWindow(cx::WindowType::Normal);

        if (!quicksandFont.openFromFile("examples/text-browser/assets/Quicksand-Medium.ttf")) {
            cx::Logger::err("Failed to load Quicksand Font");
        }

        browser.setWrapMode(cx::ui::TextWrap::CharWrap);
        browser.setMaxWidth(config.getTargetResolution().x);

        cx::ui::TextDocument doc = cx::ui::TextDocumentBuilder({ .font = defaultFont, .size = 24 })
            .setBold(true)
            .setColor(sf::Color::Green)
            .addPart("[LOG]")

            .setBold(false)
            .setColor(sf::Color::White)
            .addPart(" Hello, world!\n")

            .addPart("Discord: ")
            .setColor(sf::Color::Blue)
            .setItalic(true)
            .addPart("https://discord.gg/stejQ2zA")
            .addPart("\n")

            .setFont(quicksandFont)
            .setSize(32)
            .setItalic(false)
            .setColor(sf::Color::Magenta)
            .addPart("This is the example usage of TextBrowser")
            .addPart(", in CX Engine\n")

            .setFont(defaultFont)
            .setColor(sf::Color::Cyan)
            .setBold(true)
            .addPart(repeat("Text wrap example. ", 13))

            .build();

        browser.setDocument(doc); 
        cx::Logger::info("hello");
    }

    cx::DefaultConfigDriver config;
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void update(float dt) override {
        browser.update(dt);
    }
    void handle(const sf::Event& event) override {
        browser.handle(event);
    }
    void drawGame(sf::RenderTarget& target) override {
        target.draw(browser);
    }
    void drawUi(sf::RenderTarget& target) override {}
};

int main() {
    Game game;
    return game.run();
}

