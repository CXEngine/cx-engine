#pragma once

#include <cx-engine/systems/loading/defs.hpp>

namespace cx {

// TODO: implement loading manager
class LoadingManager {
protected:
    Vec<LoadingStep> steps;

public:
    LoadingManager() {}

    template <typename... TArgs>
    void addStep(TArgs&&... args) {
        steps.emplace_back(std::forward<TArgs>(args)...);
    }

    template <typename TSession>
    TSession getSession() {
        return TSession(steps);
    }
};

} // namespace cx
