#include <cx-engine/systems/logging/logger.hpp>

#include <SFML/Graphics/RectangleShape.hpp>
#include <cx-engine/core/app.hpp>

class GameConfigManager: public cx::AppConfigManager {
public:
    GameConfigManager() = default;

    cx::VideoMode getVideoMode() const override { return cx::VideoMode::Windowed; }

    sf::Vector2u getRenderResolution() const override { return getTargetResolution(); }
    sf::Vector2u getUiResolution() const override { return getTargetResolution(); }
    sf::Vector2u getTargetResolution() const override { return { 1920, 1080 }; }

    bool getFsrEnabled() const override { return false; }

    bool getVsyncEnabled() const override { return true; }    
    cx::Optional<uint> getFpsLimit() const override { return std::nullopt; }

    bool getFxaaEnabled() const override { return false; }
    uint getMsaaLevel() const override { return 4; }
};

class Game: public cx::App {
public:
    Game() : cx::App("Minimal CX Engine example") {
        forceRecreateWindow(cx::WindowType::Normal);
    }

    GameConfigManager config;
    cx::AppConfigManager& getConfig() override {
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

