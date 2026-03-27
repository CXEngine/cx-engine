#include <cx-engine/core/app.hpp>

#include <cx-engine/core/app/render-pipeline/fxaa.hpp>
#include <cx-engine/core/app/render-pipeline/ffx.hpp>

#include <cx-engine/systems/input/keyboard.hpp>
#include <cx-engine/systems/input/mouse.hpp>

#include <cx-engine/defs/types.hpp>

namespace cx {

App::App(String name)
    : name(std::move(name))
    , renderPipeline(*this)
    , storage(*this)
    , stateManager(currentState)
{
    Keyboard::init();
    Logger::init(storage.getLogsDir());
}

App::~App() {}

void App::updateScales() {
    uiScale = (float) getConfig().getUiResolution().y / 1080.f;
    gameScale = (float) getConfig().getRenderResolution().y / 1080.f;
    Logger::debug("Updating scales", { { "ui-scale", toString(uiScale) }, { "game-scale", toString(gameScale) } });
    stateManager.onUpdateScales();
}

void App::forceRecreateWindow(WindowType type) {
    auto& cfg = getConfig();

    sf::State state = sf::State::Windowed;
    int style = sf::Style::Default;

    String styleString, stateString;

    switch (type) {
    case WindowType::Normal:
        switch (cfg.getVideoMode()) {
        case VideoMode::Windowed:
            style = sf::Style::Default;    styleString = "sf::Style::Default";
            state = sf::State::Windowed;   stateString = "sf::State::Windowed";
            break;
        case VideoMode::Borderless:
            style = sf::Style::None;       styleString = "sf::Style::None";
            state = sf::State::Windowed;   stateString = "sf::State::Windowed";
            break;
        case VideoMode::Fullscreen:
            style = sf::Style::None;       styleString = "sf::Style::None";
            state = sf::State::Fullscreen; stateString = "sf::State::Fullscreen";
            break;
        }
        break;
    case WindowType::Splash:
        style = sf::Style::None;           styleString = "sf::Style::None";
        state = sf::State::Windowed;       stateString = "sf::State::Windowed";
        break;
    }

    sf::ContextSettings settings;
    switch (type) {
    case WindowType::Normal:
        settings.antiAliasingLevel = cfg.getMsaaLevel();
        break;
    case WindowType::Splash:
        settings.antiAliasingLevel = 0;
        break;
    }

    sf::Vector2u windowSize;
    sf::Vector2i windowPos;

    switch (type) {
    case WindowType::Normal:
        windowSize = cfg.getTargetResolution();
        windowPos = { 0, 0 };
        break;
    case WindowType::Splash:
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

        uint windowWidth = (unsigned int) (desktop.size.x * 0.5f);
        windowSize = sf::Vector2u {
            windowWidth,
            (uint) (windowWidth * 1080.f / 1920.f),
        };
        windowPos = {
            (int) ((desktop.size.x - windowSize.x) / 2),
            (int) ((desktop.size.y - windowSize.y) / 2),
        };
        break;
    }

    Logger::info("Creating/Recreating Window", {
        { "type", toString(type) },
        { "size", toString(windowSize) },
        { "style", styleString },
        { "state", stateString },
        { "msaa-lvl", toString(settings.antiAliasingLevel) },
    });

    win.close();
    win.create(sf::VideoMode(windowSize), name, style, state, settings);
    if (state == sf::State::Windowed) {
        win.setPosition(windowPos);
    }

    stateManager.onRecreateWindow();
    forceRefreshWindow(type);
}

void App::forceRefreshWindow(WindowType type) {
    auto& cfg = getConfig();

    String title = "uninit";
    Optional<uint> fpsLimit;
    bool vsyncEnabled = false;

    switch (type) {
    case WindowType::Normal:
        title = name;
        fpsLimit = cfg.getFpsLimit();
        vsyncEnabled = cfg.getVsyncEnabled();
        break;
    case WindowType::Splash:
        title = "Loading " + name;
        fpsLimit = 30u;
        vsyncEnabled = false;
        break;
    }

    Logger::info("Refreshing Window", {
        { "title", title },
        { "type", toString(type) },
        { "fps-limit", toString(fpsLimit) },
        { "vsync", toString(vsyncEnabled) },
    });

    win.setTitle(title);
    if (fpsLimit) {
        win.setFramerateLimit(*fpsLimit);
    } else {
        win.setFramerateLimit(0);
    }
    win.setVerticalSyncEnabled(vsyncEnabled);

    sf::Vector2u renderTextureResolution;
    switch (type) {
    case WindowType::Normal:
        renderTextureResolution = cfg.getRenderResolution();
        break;
    case WindowType::Splash:
        renderTextureResolution = win.getSize();
        break;
    }

    Logger::info("Recreating/Creating target render texture", {
        { "resolution", toString(renderTextureResolution) }
    });

    renderTexture = sf::RenderTexture(renderTextureResolution);
    renderPipeline.clear();
    if (cfg.getFxaaEnabled()) {
        renderPipeline.addPass(new FxaaPass(*this));
    }
    if (cfg.getFsrEnabled()) {
        renderPipeline.addPass(new FfxPass(*this));
    }

    stateManager.onRefreshWindow();
    updateScales();
}

void App::forceQuit() {
    stateManager.clear();
    mainLoop.stop();
}

void App::handle(const sf::Event& event) {
    stateManager.handle(event);
}

void App::update(float dt) {
    stateManager.update(dt);
    stateManager.applyPending();
}

void App::drawGame(sf::RenderTarget& target) {
    stateManager.drawGame(target);
}

void App::drawUi(sf::RenderTarget& target) {
    stateManager.drawUi(target);
}

int App::run() {
    mainLoop.start();
    while (mainLoop.isRunning()) {
        mainLoop.next();
        float dt = mainLoop.getDeltaTime();

        gamepads.scan();
        gamepads.update();

        Keyboard::update();
        Mouse::update();
        while (auto event = win.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mainLoop.stop();
            } else if (event->is<sf::Event::Resized>()) {
                updateScales();
            }
            Keyboard::handle(*event);
            Mouse::handle(*event);
            handle(*event);
        }

        update(dt);

        renderTexture.clear();
        drawGame(renderTexture);
        renderTexture.display();

        renderPipeline.execute(renderTexture, win);

        drawUi(win);

        win.display();

        while (mainLoop.hasCommands()) {
            cx::MainLoopCommand cmd = mainLoop.popCommand();
            switch (cmd.type) {
            case cx::MainLoopCommandType::Quit:
                forceQuit();
                break;
            case cx::MainLoopCommandType::RecreateWindow:
                forceRecreateWindow(std::get<WindowType>(cmd.data));
                break;
            case cx::MainLoopCommandType::RefreshWindow:
                forceRefreshWindow(std::get<WindowType>(cmd.data));
                break;
            }
        }

        mainLoop.finishFrame();
    }

    win.close();
    return 0;
}

}
