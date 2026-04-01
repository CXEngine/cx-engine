
#include <cx-engine/core/app.hpp>

#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/app/config-drivers/default.hpp>

#include <cx-engine/ui/text-browser.hpp>
#include <cx-engine/ui/text/builder.hpp>
#include <cx-engine/ui/scroll-view.hpp>
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
    cx::ui::ScrollView<cx::ui::TextBrowser> scrollView;
    sf::Font quicksandFont;

    Game() : cx::App("Text browser example") {
        forceRecreateWindow(cx::WindowType::Normal);

        scrollView.getContent().setWrapMode(cx::ui::TextWrap::CharWrap);
        scrollView.getContent().setMaxWidth(config.getTargetResolution().x);

        scrollView.setSize(sf::Vector2f(config.getTargetResolution()));

        cx::ui::TextDocument doc = cx::ui::TextDocumentBuilder({ .font = defaultFont, .size = 36 })
            .setColor(sf::Color::Cyan)
            .setBold(true)
            .addPart(repeat("Text wrap example.\n", 100))

            .build();

        scrollView.getContent().setDocument(doc); 
        cx::Logger::info("hello");
    }

    cx::DefaultConfigDriver config;
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void update(float dt) override {
        scrollView.update(dt);
    }
    void handle(const sf::Event& event) override {
        scrollView.handle(event);
    }
    void drawGame(sf::RenderTarget& target) override {
        target.draw(scrollView);
    }
    void drawUi(sf::RenderTarget& target) override {}
};

int main() {
    Game game;
    return game.run();
}


