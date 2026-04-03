#include <cx-engine/utils/hybrid-str.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx {

struct LoadingStep {
    HybridStr desc;
    UniquePtr<LoadingStep> substeps;
    Func<void()> task;
    float weight = 1.0f;
};

}
