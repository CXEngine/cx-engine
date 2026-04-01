/// @file bitflags.hpp
/// @brief Utilities for enabling type-safe bitwise operators on enums.

#pragma once
#include <type_traits>

namespace cx {

/// @brief Internal implementations for bitflags operators to keep the macro lean.
namespace bitflagsImpl {
    template <typename T>
    using Underlying = std::underlying_type_t<T>;

    template <typename T>
    constexpr T orOp(T lhs, T rhs) {
        return static_cast<T>(static_cast<Underlying<T>>(lhs) | static_cast<Underlying<T>>(rhs));
    }

    template <typename T>
    constexpr T andOp(T lhs, T rhs) {
        return static_cast<T>(static_cast<Underlying<T>>(lhs) & static_cast<Underlying<T>>(rhs));
    }

    template <typename T>
    constexpr T xorOp(T lhs, T rhs) {
        return static_cast<T>(static_cast<Underlying<T>>(lhs) ^ static_cast<Underlying<T>>(rhs));
    }

    template <typename T>
    constexpr T notOp(T rhs) {
        return static_cast<T>(~static_cast<Underlying<T>>(rhs));
    }

    template <typename T>
    constexpr T lshiftOp(T lhs, int rhs) {
        return static_cast<T>(static_cast<Underlying<T>>(lhs) << rhs);
    }

    template <typename T>
    constexpr T rshiftOp(T lhs, int rhs) {
        return static_cast<T>(static_cast<Underlying<T>>(lhs) >> rhs);
    }
}

/// @brief Checks if a specific flag is set in the given value.
/// @param value The value to check
/// @param flag The flag to look for
/// @return True if the flag is set, false otherwise
template <typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, bool>
hasFlag(T value, T flag) {
    using U = std::underlying_type_t<T>;
    return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);
}

/// @brief Sets or clears a specific flag in the given value.
/// @param value Reference to the value to modify
/// @param flag The flag to set or clear
/// @param enabled True to set the flag, false to clear it
template <typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, void>
setFlag(T& value, T flag, bool enabled = true) {
    if (enabled) {
        value = bitflagsImpl::orOp(value, flag);
    } else {
        value = bitflagsImpl::andOp(value, bitflagsImpl::notOp(flag));
    }
}

} // namespace cx

/// @brief Macro to enable bitflags operators for a specific enum type.
/// Should be used in the same namespace as the enum.
/// @param T The enum type to enable operators for
#define TD_BITFLAGS(T)                                                                  \
    inline constexpr T operator|(T lhs, T rhs) { return cx::bitflagsImpl::orOp(lhs, rhs); } \
    inline constexpr T operator&(T lhs, T rhs) { return cx::bitflagsImpl::andOp(lhs, rhs); } \
    inline constexpr T operator^(T lhs, T rhs) { return cx::bitflagsImpl::xorOp(lhs, rhs); } \
    inline constexpr T operator~(T rhs) { return cx::bitflagsImpl::notOp(rhs); }            \
    inline constexpr T& operator|=(T& lhs, T rhs) { return lhs = lhs | rhs; }           \
    inline constexpr T& operator&=(T& lhs, T rhs) { return lhs = lhs & rhs; }           \
    inline constexpr T& operator^=(T& lhs, T rhs) { return lhs = lhs ^ rhs; }           \
    inline constexpr T operator<<(T lhs, int rhs) { return cx::bitflagsImpl::lshiftOp(lhs, rhs); } \
    inline constexpr T operator>>(T lhs, int rhs) { return cx::bitflagsImpl::rshiftOp(lhs, rhs); } \
    inline constexpr T& operator<<=(T& lhs, int rhs) { return lhs = lhs << rhs; }       \
    inline constexpr T& operator>>=(T& lhs, int rhs) { return lhs = lhs >> rhs; }       \
    inline constexpr bool operator!(T rhs) { return static_cast<std::underlying_type_t<T>>(rhs) == 0; }
