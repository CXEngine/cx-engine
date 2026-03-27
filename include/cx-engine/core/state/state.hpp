#pragma once

#include <cx-engine/utils/bitflags.hpp>

#include <SFML/Window/Event.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace cx {

enum class StateFlags {
    None        = 1 << 0,

    Transparent = 1 << 1,
    AllowUpdate = 1 << 2,
    AllowHandle = 1 << 3,

    CoverDraw   = 1 << 4,
    CoverUpdate = 1 << 5,
    CoverHandle = 1 << 6,
};

TD_BITFLAGS(StateFlags);

class State: public sf::Drawable {
protected:
    StateFlags flags;

    State(StateFlags flags) : flags(flags) {}

public:
    virtual ~State() = default;

    State(const State&) = delete;
    State& operator=(const State&) = delete;

    virtual StateFlags getFlags() { return flags; }
    virtual bool hasFlag(StateFlags flag) { return bool(flags & flag); }

    virtual void onUpdateScales() {}
    virtual void onRecreateWindow() {}
    virtual void onRefreshWindow() {}

    virtual void onShow() {}
    virtual void onCover(State* newState) {}

    virtual void coverDrawGame(sf::RenderTarget& target) const {}
    virtual void coverDrawUi(sf::RenderTarget& target) const {}
    virtual void coverUpdate(float dt) {}
    virtual void coverHandle(const sf::Event& event) {}

    virtual void update(float dt) = 0;
    virtual void handle(const sf::Event& event) = 0;

    virtual void drawGame(sf::RenderTarget& target) const {}
    virtual void drawUi(sf::RenderTarget& target) const {}

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override {
        drawGame(target);
        drawUi(target);
    }
};

}
