#pragma once
#include <type_traits>

namespace cx {

template <typename Enum>
struct _td_EnableBitflagsOperators {
    static constexpr bool enable = false;
};

#define TD_BITFLAGS(x)                    \
template <>                               \
struct _td_EnableBitflagsOperators<x> {   \
    static constexpr bool enable = true;  \
};

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator|(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) |
        static_cast<Underlying>(rhs));
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator&(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) &
        static_cast<Underlying>(rhs));
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator^(Enum lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) ^
        static_cast<Underlying>(rhs));
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator~(Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        ~static_cast<Underlying>(rhs));
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator|=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) |
        static_cast<Underlying>(rhs));
    return lhs;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator&=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) &
        static_cast<Underlying>(rhs));
    return lhs;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator^=(Enum& lhs, Enum rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) ^
        static_cast<Underlying>(rhs));
    return lhs;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator<<(Enum lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) << rhs);
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum>
operator>>(Enum lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
        static_cast<Underlying>(lhs) >> rhs);
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator<<=(Enum& lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) << rhs);
    return lhs;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, Enum&>
operator>>=(Enum& lhs, int rhs) {
    using Underlying = std::underlying_type_t<Enum>;
    lhs = static_cast<Enum>(
        static_cast<Underlying>(lhs) >> rhs);
    return lhs;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, bool>
operator!(Enum rhs) {
    return static_cast<std::underlying_type_t<Enum>>(rhs) == 0;
}

template <typename Enum>
constexpr std::enable_if_t<_td_EnableBitflagsOperators<Enum>::enable, bool>
hasFlag(Enum value, Enum flag) {
    using Underlying = std::underlying_type_t<Enum>;
    return (static_cast<Underlying>(value) & static_cast<Underlying>(flag)) == static_cast<Underlying>(flag);
}

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
