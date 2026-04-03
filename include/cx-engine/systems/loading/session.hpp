#pragma once

#include <cx-engine/systems/loading/defs.hpp>

namespace cx {

/// @brief Represents an active loading session
class LoadingSession {
public:
    virtual ~LoadingSession() = default;

    /// @brief Polls for the next event from the internal queue
    /// @return An Optional containing the event if one was available, or None
    virtual Optional<LoadingEvent> pollEvent() = 0;

    /// @brief Returns the current global progress (0.0 to 1.0)
    /// @return The global progress
    virtual float getProgress() const = 0;

    /// @brief Checks whether the loading process is finished
    /// @return true if the loading process is finished false otherwise
    virtual bool isFinished() const = 0;

    /// @brief Blocks the calling thread until the loading process is complete
    virtual void wait() = 0;
};

} // namespace cx
