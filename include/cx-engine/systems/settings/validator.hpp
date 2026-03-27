#pragma once
#include <cx-engine/defs/types.hpp>

#include <type_traits>

namespace cx {

template<typename V, typename T>
concept ValidatorFor =
    requires(const V& v, const T& x) {
        { v.validate(x) } -> std::convertible_to<bool>;
    };

template <typename T>
struct NoValidator {
    bool validate(const T&) const {
        return true;
    }
};

template <typename T>
struct IsOptional: std::false_type {};

template <typename T>
struct IsOptional<Optional<T>>: std::true_type {};

template <typename T>
struct Range {
    T min;
    T max;

    bool validate(const T& v) const {
        return v >= min && v <= max;
    }
};

}
