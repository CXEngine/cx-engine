#include <cx-engine/core/app.hpp>

#include <cx-engine/core/app/config-drivers/default.hpp>
#include <cx-engine/systems/logging/logger.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

class Game: public cx::App {
public:
    Game() : cx::App("CX Engine loading example") {
        forceRecreateWindow(cx::WindowType::Normal);
    }

    cx::DefaultConfigDriver config;
    cx::ConfigDriver& getConfig() override {
        return config;
    }
};

int main() {
    Game game;
    return game.run();
}

