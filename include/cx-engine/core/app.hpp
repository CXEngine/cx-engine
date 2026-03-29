#pragma once

#include <SFML/Graphics/Font.hpp>
#include <cx-engine/defs/types.hpp>

#include <cx-engine/core/app/render-pipeline.hpp>
#include <cx-engine/core/app/config-driver.hpp>

#include <cx-engine/systems/assets/cxpk.hpp>
#include <cx-engine/utils/buf.hpp>

#include <cx-engine/core/app/win-type.hpp>
#include <cx-engine/core/main-loop.hpp>

#include <cx-engine/core/state/state-manager.hpp>
#include <cx-engine/core/state/state.hpp>

#include <cx-engine/utils/dynarena.hpp>

#include <cx-engine/systems/global-texture-atlas.hpp>

#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/systems/storage-manager.hpp>

#include <cx-engine/systems/gamepad/gamepad-manager.hpp>
#include <cx-engine/systems/gamepad/gamepad.hpp>
#include <cx-engine/systems/gamepad/gamepad-icons.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace cx {

class App {
public:
    String name;

    sf::RenderWindow win;
    sf::View view;

    RenderPipeline renderPipeline;
    sf::RenderTexture renderTexture;

    DynArena arena;
    MainLoop mainLoop;

    GlobalSpriteAtlas globalSpriteAtlas;

    GamepadManager gamepads;
    GamepadIcons gamepadIcons;

    sf::Font defaultFont;

    StorageManager storage;

    StateManager stateManager;
    State* currentState = nullptr;

    float uiScale = 1.0f;
    float gameScale = 1.0f;

    App(String name);
    ~App();

    virtual ConfigDriver& getConfig() = 0;

    void updateScales();

    void forceRecreateWindow(WindowType type = WindowType::Normal);
    void forceRefreshWindow(WindowType type = WindowType::Normal);
    void forceQuit();

    int run();

    virtual void update(float dt);
    virtual void handle(const sf::Event& event);
    virtual void drawGame(sf::RenderTarget& target);
    virtual void drawUi(sf::RenderTarget& target);
};

}
