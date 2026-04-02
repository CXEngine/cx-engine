
#include <cx-engine/core/app.hpp>

#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/app/config-drivers/default.hpp>

#include <cx-engine/ui/text-browser.hpp>
#include <cx-engine/ui/text/builder.hpp>
#include <cx-engine/ui/scroll-view.hpp>
#include <cx-engine/ui/ui-widget.hpp>

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

cx::String repeat(cx::StringView s, cx::usize times) {
    cx::String result;
    result.reserve(s.length() * times);
    for (cx::usize i = 0; i < times; ++i) {
        result += s;
    }
    return result;
}

cx::ui::TextDocument makeDocument(const sf::Font& font, cx::uint howMany) {
    cx::ui::TextDocumentBuilder b({ .font = font, .size = 36 });
    for (cx::uint i = 0; i < howMany; ++i) {
        b.setColor({ randColorChan(), randColorChan(), randColorChan(), 255 });
        b.addPart("Line #" + cx::toString(i) + ". ");
        b.addPart(getRandomSentence());
        b.addPart(repeat(" ", 70));
        b.addPart("Easter egg!");
        b.addPart("\n");
    }

    return std::move(b).build();
}

class Game: public cx::App {
public:
    cx::ui::ScrollView<cx::ui::TextBrowser> scrollView;
    sf::Font quicksandFont;

    Game() : cx::App("Scroll view example") {}

    void init() override {
        forceRecreateWindow(cx::WindowType::Normal);

        scrollView.getContent().setWrapMode(cx::ui::TextWrap::NoWrap);

        scrollView.smoothScrollSpeed = { 25.f, 25.f };
        scrollView.mouseScrollSpeed = { 120.f, 120.f };
        scrollView.setInputMode(cx::ui::InputMode::All);
        scrollView.setSize(sf::Vector2f(config.getTargetResolution()));

        srand(time(NULL));
        cx::ui::TextDocument doc = makeDocument(defaultFont, 150);

        scrollView.getContent().setDocument(doc); 
        cx::Logger::info("hello");
    }

    cx::DefaultConfigDriver config;
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void update(float dt) override {
        scrollView.update(dt);
        for (auto& gamepad: gamepads) {
            scrollView.gamepad(gamepad);
        }
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


