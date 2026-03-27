#pragma once

#include <cx-engine/core/app/win-type.hpp>

#include <cx-engine/defs/types.hpp>
#include <cx-engine/utils/buf.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

namespace cx {

enum class MainLoopCommandType {
    Quit,
    RecreateWindow,
    RefreshWindow,
};

struct MainLoopCommand {
    MainLoopCommandType type;
    NullableUnion<WindowType> data;
};

constexpr usize MaxMainLoopCommands = 16;

class MainLoop {
private:
    using CommandQueue = Buf<MainLoopCommand, MaxMainLoopCommands>;
    CommandQueue commandQueue;

    float deltaTime;
    float currentFramerate;

    bool running;
    sf::Clock frameClock;

public:
    explicit MainLoop()
        : deltaTime(0.0f), currentFramerate(0.0f), running(false) {
    }

    bool isRunning() const { return running; }

    void start() {
        running = true;
        deltaTime = 0.0f;
        currentFramerate = 0.0f;
        frameClock.restart();
    }

    void stop() {
        running = false;
    }

    bool next() {
        if (!running) return false;

        deltaTime = frameClock.restart().asSeconds();
        if (deltaTime > 0.0f) {
            currentFramerate = 1.0f / deltaTime;
        } else {
            currentFramerate = 0.0f;
        }

        return running;
    }

    void finishFrame() {
        if (!running) return;
    }

    float getDeltaTime() const {
        return deltaTime;
    }

    float getFramerate() const {
        return currentFramerate;
    }

    float getUptime() const {
        return frameClock.getElapsedTime().asSeconds();
    }

    bool hasCommands() const {
        return !commandQueue.isEmpty();
    }

    MainLoopCommand popCommand() {
        return commandQueue.pop();
    }

    void requestQuit() {
        if (commandQueue.isFull()) return;
        commandQueue.emplace(MainLoopCommandType::Quit);
    }

    void requestRecreateWindow(WindowType type = WindowType::Normal) {
        if (commandQueue.isFull()) return;
        commandQueue.emplace(MainLoopCommandType::RecreateWindow, type);
    }

    void requestRefreshWindow(WindowType type = WindowType::Normal) {
        if (commandQueue.isFull()) return;
        commandQueue.emplace(MainLoopCommandType::RefreshWindow, type);
    }
};

}
