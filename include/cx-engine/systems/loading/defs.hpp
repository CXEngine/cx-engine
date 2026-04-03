#include <cx-engine/utils/hybrid-str.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx {

struct LoadingStep {
    HybridStr desc;
    Vec<LoadingStep> substeps;
    Func<void()> task;
    float weight = 1.0f;
};

/// @brief Represents an event occurring during the loading process.
struct LoadingEvent {
    enum class Type {
        StepBegin, ///< Entering a new step or category
        StepEnd,   ///< Finishing a step or category
        Progress   ///< Global progress update
    };

    Type type;
    const LoadingStep* step = nullptr;  ///< Pointer to the relevant step (null for global progress)
    float progress = 0.0f;              ///< Current global progress (0.0 to 1.0)
};

}
