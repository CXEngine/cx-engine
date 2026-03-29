#include <cx-engine/core/app.hpp>

#include <cx-engine/core/app/config-drivers/default.hpp>
#include <cx-engine/systems/logging/logger.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

class Game: public cx::App {
public:
    Game() : cx::App("Minimal CX Engine example") {
        forceRecreateWindow(cx::WindowType::Normal);
    }

    cx::DefaultConfigDriver config;
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void update(float dt) override {}
    void handle(const sf::Event& event) override {}
    void drawGame(sf::RenderTarget& target) override {
        sf::RectangleShape rect({ 200, 200 });
        rect.setPosition({
            (float) config.getTargetResolution().x / 2.0f - (rect.getSize().x / 2.0f),
            (float) config.getTargetResolution().y / 2.0f - (rect.getSize().y / 2.0f)
        });
        target.draw(rect);
    }
    void drawUi(sf::RenderTarget& target) override {}
};

int main() {
    Game game;
    return game.run();
}

