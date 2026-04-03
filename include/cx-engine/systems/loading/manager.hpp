#include <cx-engine/systems/loading/defs.hpp>

namespace cx {

// TODO: implement loading manager
class LoadingManager {
protected:
    Vec<LoadingStep> steps;

public:
    LoadingManager() {}

    template <typename... Args>
    void addStep(Args&&... args) {
        steps.emplace_back(std::forward<Args>(args)...);
    }
};

} // namespace cx
