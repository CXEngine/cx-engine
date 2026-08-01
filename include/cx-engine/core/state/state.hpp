#pragma once

#include <cx-engine/utils/bitflags.hpp>

#include <SFML/Window/Event.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace cx {

    /// Flags that control how a State behaves within a @ref StateManager.
enum class StateFlags {
    None        = 1 << 0,

    /// Allows states below this one to remain visible.
    /// Useful for overlays such as pause menus or HUDs.
    Transparent = 1 << 1,

    /// Allows states below this one to continue updating.
    /// Useful for overlays that should not pause gameplay.
    UpdateBelow = 1 << 2,

    /// Allows states below this one to continue receiving input events.
    PassthroughEvents = 1 << 3,

    /// Calls @ref State::coverDrawGame and @ref State::coverDrawUi while this
    /// state is covered by another state.
    CoverDraw   = 1 << 4,

    /// Calls @ref State::coverUpdate while this state is covered by another state.
    CoverUpdate = 1 << 5,
    /// Calls @ref State::coverHandle while this state is covered by another state.
    CoverHandle = 1 << 6,
};

TD_BITFLAGS(StateFlags);

/// @brief Base class for engine states.
///
/// A state represents one logical layer of the game, for example
/// you can have a state that handles the game logic or a state
/// that displays the pause menu.
/// States are managed by a @ref StateManager and exist in a stack.
///
/// The behavior when states are covered by other states is controlled by @ref StateFlags.
class State {
protected:
    StateFlags flags;

    State(StateFlags flags) : flags(flags) {}

public:
    virtual ~State() = default;

    State(const State&) = delete;
    State& operator=(const State&) = delete;

    virtual StateFlags getFlags() { return flags; }
    virtual bool hasFlag(StateFlags flag) { return bool(flags & flag); }

    /// Called when the application's UI/rendering scale changes.
    virtual void onUpdateScales() {}
    /// Called when the application window is recreated.
    virtual void onRecreateWindow() {}
    /// Called when the application window is refreshed.
    virtual void onRefreshWindow() {}

    /// Called when this state becomes the top of the stack.
    virtual void onShow() {}
    /// Called when a new state is pushed on top of this state.
    virtual void onCover(State* newState) {}

    /// Called to draw game elements when covered.
    virtual void coverDrawGame(sf::RenderTarget& target) const {}
    /// Called to draw UI elements when covered.
    virtual void coverDrawUi(sf::RenderTarget& target) const {}
    /// Called to update logic when covered.
    virtual void coverUpdate(float dt) {}
    /// Called to handle input events when covered.
    virtual void coverHandle(const sf::Event& event) {}

    /// Pure virtual function to update game logic.
    virtual void update(float dt) = 0;
    /// Pure virtual function to handle input events.
    virtual void handle(const sf::Event& event) = 0;

    /// Called to draw game elements.
    virtual void drawGame(sf::RenderTarget& target) const {}
    /// Called to draw UI elements.
    virtual void drawUi(sf::RenderTarget& target) const {}
};

}
