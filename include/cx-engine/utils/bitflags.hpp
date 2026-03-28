/// @file bitflags.hpp
/// @brief Utilities for enabling type-safe bitwise operators on enums.

#pragma once
#include <type_traits>

namespace cx {

/// @brief Internal trait to enable bitwise operators for an enum.
template <typename Enum>
struct _td_EnableBitflagsOperators {
    static constexpr bool enable = false; //< Whether bitwise operators are enabled
};

/// @brief Macro to enable bitflags operators for a specific enum type.
/// @param x The enum type to enable operators for
#define TD_BITFLAGS(x)                    \
template <>                               \
struct _td_EnableBitflagsOperators<x> {   \
    static constexpr bool enable = true;  \
};

/// @brief Performs bitwise OR on two enum values
/// @param lhs Left-hand side operand
/// @param rhs Right-hand side operand
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator|(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) |
        static_cast<Underlying>(rhs));
}

/// @brief Performs bitwise AND on two enum values
/// @param lhs Left-hand side operand
/// @param rhs Right-hand side operand
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator&(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) &
        static_cast<Underlying>(rhs));
}

/// @brief Performs bitwise XOR on two enum values
/// @param lhs Left-hand side operand
/// @param rhs Right-hand side operand
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator^(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) ^
        static_cast<Underlying>(rhs));
}

/// @brief Performs bitwise NOT on an enum value
/// @param rhs The enum value to negate
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator~(Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        ~static_cast<Underlying>(rhs));
}

/// @brief Performs bitwise OR and assignment on an enum value
/// @param lhs Reference to the left-hand side operand
/// @param rhs Right-hand side operand
/// @return Reference to the modified lhs
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator|=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) |
        static_cast<Underlying>(rhs));
    return lhs;
}

/// @brief Performs bitwise AND and assignment on an enum value
/// @param lhs Reference to the left-hand side operand
/// @param rhs Right-hand side operand
/// @return Reference to the modified lhs
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator&=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) &
        static_cast<Underlying>(rhs));
    return lhs;
}

/// @brief Performs bitwise XOR and assignment on an enum value
/// @param lhs Reference to the left-hand side operand
/// @param rhs Right-hand side operand
/// @return Reference to the modified lhs
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator^=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) ^
        static_cast<Underlying>(rhs));
    return lhs;
}

/// @brief Performs left shift on an enum value
/// @param lhs The enum value to shift
/// @param rhs Number of positions to shift
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator<<(Enum lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) << rhs);
}

/// @brief Performs right shift on an enum value
/// @param lhs The enum value to shift
/// @param rhs Number of positions to shift
/// @return Resulting enum value
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator>>(Enum lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) >> rhs);
}

/// @brief Performs left shift and assignment on an enum value
/// @param lhs Reference to the enum value to shift
/// @param rhs Number of positions to shift
/// @return Reference to the modified lhs
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator<<=(Enum& lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) << rhs);
    return lhs;
}

/// @brief Performs right shift and assignment on an enum value
/// @param lhs Reference to the enum value to shift
/// @param rhs Number of positions to shift
/// @return Reference to the modified lhs
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator>>=(Enum& lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) >> rhs);
    return lhs;
}

/// @brief Logical NOT operator for enums (checks if value is zero)
/// @param rhs The enum value to check
/// @return True if the value is zero, false otherwise
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, bool>
operator!(Enum rhs) {
    return static_cast<std::underlying_type_t<Enum>>(rhs) == 0;
}

/// @brief Checks if a specific flag is set in the given value
/// @param value The value to check
/// @param flag The flag to look for
/// @return True if the flag is set, false otherwise
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, bool>
hasFlag(Enum value, Enum flag) {
    using Underlying = std::underlying_type_t<Enum>;
    return (static_cast<Underlying>(value) & static_cast<Underlying>(flag)) == static_cast<Underlying>(flag);
}

/// @brief Sets or clears a specific flag in the given value
/// @param value Reference to the value to modify
/// @param flag The flag to set or clear
/// @param enabled True to set the flag, false to clear it
template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, void>
setFlag(Enum& value, Enum flag, bool enabled = true) {
    if (enabled) {
        value |= flag;
    } else {
        value &= ~flag;
    }
}

}
