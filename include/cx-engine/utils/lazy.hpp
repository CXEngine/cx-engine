#pragma once
#include <cx-engine/defs/types.hpp>

namespace cx {

/// @brief A wrapper for lazy initialization of a value.
///
/// Lazy allows for deferring the computation of a value until it is actually
/// needed. Once computed, the value is cached for subsequent access.
///
/// @tparam T Type of the value to be lazily initialized
template<typename T>
class Lazy {
protected:
    mutable Optional<T> value; //< Cached value, if already computed
    Func<T()> computeFn;       //< Function used to compute the value

public:
    /// @brief Creates an empty Lazy instance without a computation function.
    Lazy() = default;

    /// @brief Creates a Lazy instance with the specified computation function
    /// @param fn Function used to compute the value when requested
    explicit Lazy(Func<T()> fn)
        : computeFn(std::move(fn)) {}

    /// @brief Gets the lazily initialized value, computing it if necessary
    /// @return Constant reference to the initialized value
    /// @note If the value was not yet computed, computeFn will be called.
    const T& get() const {
        if (!value) {
            value = computeFn();
        }
        return *value;
    }

    /// @brief Resets the Lazy instance, discarding the cached value.
    ///
    /// Subsequent calls to get() will trigger a re-computation using computeFn.
    void reset() const {
        value.reset();
    }
};

} // namespace cx
