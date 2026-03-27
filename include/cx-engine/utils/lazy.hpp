#pragma once
#include <cx-engine/defs/types.hpp>

namespace cx {

template<typename T>
class Lazy {
public:
    Lazy() = default;

    explicit Lazy(Func<T()> fn)
        : computeFn(std::move(fn)) {}

    const T& get() const {
        if (!value) {
            value = computeFn();
        }
        return *value;
    }

    void reset() const {
        value.reset();
    }

private:
    mutable Optional<T> value;
    Func<T()> computeFn;
};

}
