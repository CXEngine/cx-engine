#pragma once

#include <cx-engine/core/state/state.hpp>
#include <cx-engine/defs/types.hpp>

#include <cx-engine/utils/buf.hpp>

namespace cx {

/// @brief Manages a stack of State objects and controls their lifecycle and behavior.
///
/// The StateManager orchestrates state updates, input handling, and rendering.
/// It allows states to be pushed, popped, or cleared from the stack. To ensure
/// safe modifications during the update loop, state changes can be deferred using
/// the request methods and applied via applyPending(). This is done automatically
/// by the @ref App class so in most cases you shouldn't use this api directly,
class StateManager {
private:
    static constexpr usize MaxPendingOps = 10;

    enum class PendingOpType {
        Push,
        Pop,
        Clear
    };

    struct PendingOp {
        PendingOpType type;
        State* state = nullptr;
    };

    Vec<State*> stateStack;
    State*& stateStackTop;

    Buf<PendingOp, MaxPendingOps> pendingBuf;

private:
    void applyPush(State* state) {
        if (auto* current = currentState()) {
            current->onCover(state);
        }
        stateStack.push_back(state);
        stateStackTop = state;
        state->onShow();
    }

    void applyPop() {
        if (!stateStack.empty()) {
            delete stateStack.back();
            stateStack.pop_back();
        }

        stateStackTop = currentState();

        if (auto* current = currentState()) {
            current->onShow();
        }
    }

public:
    /// Constructs the StateManager with a reference to the top state pointer.
    StateManager(State*& stateStackTop)
        : stateStackTop(stateStackTop) {}

    /// Destructor, clears all states in the stack.
    ~StateManager() {
        clear();
    }

    /// Requests to push a new state onto the stack.
    void requestPush(State* state) {
        if (pendingBuf.isFull()) return;
        pendingBuf.emplace(PendingOpType::Push, state);
    }

    /// Requests to pop the top state from the stack.
    void requestPop() {
        if (pendingBuf.isFull()) return;
        pendingBuf.emplace(PendingOpType::Pop);
    }

    /// Requests to clear all states from the stack.
    void requestClear() {
        if (pendingBuf.isFull()) return;
        pendingBuf.emplace(PendingOpType::Clear);
    }

    /// Immediately pushes a new state onto the stack.
    /// @warning Unsafe if called from a state's update/draw/... method
    /// @see requestPush
    inline void forcePush(State* state) { applyPush(state); }

    /// Immediately pops the top state from the stack.
    /// @warning Unsafe if called from a state's update/draw/... method
    /// @see requestPop
    inline void forcePop() { applyPop(); }

    /// Applies all pending operations added via
    /// @ref requestPop, @ref requestPush and @ref requestClear
    void applyPending() {
        for (usize i = 0; i < pendingBuf.getElementCount(); ++i) {
            const PendingOp& op = pendingBuf[i];

            switch (op.type) {
            case PendingOpType::Push:
                applyPush(op.state);
                break;

            case PendingOpType::Pop:
                applyPop();
                break;

            case PendingOpType::Clear:
                clear();
                break;
            }
        }

        pendingBuf.clear();
    }


    State* stateAtTop() {
        return stateStack.empty() ? nullptr : stateStack.back();
    }
    State* currentState() {
        return stateAtTop();
    }

    void handle(const sf::Event& event) {
        for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) {
            State* state = *it;

            if (state == stateAtTop()) {
                state->handle(event);
            } else if (state->hasFlag(StateFlags::CoverHandle)) {
                state->coverHandle(event);
            }

            if (!state->hasFlag(StateFlags::AllowUpdate)) break;
        }
    }

    void update(float dt) {
        for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) {
            State* state = *it;

            if (state == stateAtTop()) {
                state->update(dt);
            } else if (state->hasFlag(StateFlags::CoverUpdate)) {
                state->coverUpdate(dt);
            }

            if (!state->hasFlag(StateFlags::AllowUpdate)) {
                break;
            }
        }
    }

    void drawGame(sf::RenderTarget& target) {
        auto begin = stateStack.begin();

        for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) {
            if (!(*it)->hasFlag(StateFlags::Transparent)) {
                begin = std::next(it).base();
                break;
            }
        }

        for (auto it = begin; it != stateStack.end(); ++it) {
            State* state = *it;

            if (state == stateAtTop()) {
                state->drawGame(target);
            } else if (state->hasFlag(StateFlags::CoverDraw)) {
                state->coverDrawGame(target);
            }
        }
    }

    void drawUi(sf::RenderTarget& target) {
        auto begin = stateStack.begin();

        for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it) {
            if (!(*it)->hasFlag(StateFlags::Transparent)) {
                begin = std::next(it).base();
                break;
            }
        }

        for (auto it = begin; it != stateStack.end(); ++it) {
            State* state = *it;

            if (state == stateAtTop()) {
                state->drawUi(target);
            } else if (state->hasFlag(StateFlags::CoverDraw)) {
                state->coverDrawUi(target);
            }
        }
    }

    void onUpdateScales() {
        for (auto& state: stateStack) {
            state->onUpdateScales();
        }
    }
    void onRecreateWindow() {
        for (auto& state: stateStack) {
            state->onRecreateWindow();
        }
    }
    void onRefreshWindow() {
        for (auto& state: stateStack) {
            state->onRefreshWindow();
        }
    }

    void clear() {
        while (!stateStack.empty()) {
            delete stateStack.back();
            stateStack.pop_back();
        }
        stateStackTop = nullptr;
    }

    State* stateAt(usize index) {
        return stateStack[index];
    }
    const State* stateAt(usize index) const {
        return stateStack[index];
    }

    State* lastState() {
        if (stateStack.size() < 2) return nullptr;
        return stateStack[stateStack.size() - 2];
    }
    const State* lastState() const {
        if (stateStack.size() < 2) return nullptr;
        return stateStack[stateStack.size() - 2];
    }
};

} // namespace cx
