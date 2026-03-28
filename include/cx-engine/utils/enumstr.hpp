/**
 * @file enumstr.hpp
 * @brief Utilities for converting enums to strings and vice versa.
 */

#pragma once

#include <cx-engine/utils/strconv.hpp>
#include <cx-engine/defs/types.hpp>

namespace cx {

/// @brief Trait defining string representations for an enum type.
/// @tparam Enum The enum type
template <typename Enum>
struct EnumStrings {
    /// @brief Gets the list of string names for the enum
    /// @return Slice of string views containing the names
    static const Slice<StringView> names();
};

/// @brief Function pointer type for retrieving enum names.
using EnumNamesFn = const Slice<StringView> (*)();

/// @brief Global registry for enum string retrieval by type index.
inline HashMap<TypeIndex, EnumNamesFn> enumStringsRegistry; //< Registry mapping type indices to name functions

/// @brief Registers an enum type in the global string registry
/// @tparam Enum The enum type to register
template <typename Enum>
inline void registerEnumStrings() {
    enumStringsRegistry[typeid(Enum)] = &EnumStrings<Enum>::names;
}

/// @brief Helper struct for automatic enum string registration.
/// @tparam Enum The enum type to register
template <typename Enum>
struct EnumStringsAutoRegister {
    /// @brief Constructs the helper and registers the enum strings
    EnumStringsAutoRegister() {
        registerEnumStrings<Enum>();
    }
};

/// @brief Retrieves the string names for a given type index
/// @param type The type index of the enum
/// @return Slice of string views, or an empty slice if not found
inline const Slice<StringView> getEnumStrings(const TypeIndex& type) {
    auto it = enumStringsRegistry.find(type);
    if (it == enumStringsRegistry.end()) {
        return Slice<StringView>();
    }
    return it->second();
}

/// @brief Converts an enum value to its string representation
/// @tparam Enum The enum type
/// @param value The enum value to convert
/// @return String representation of the value
/// @throws StrconvError if the value is out of range
template <typename Enum>
String enumToString(Enum value) {
    auto names = EnumStrings<Enum>::names();
    auto idx = static_cast<usize>(value);
    if (idx < names.size()) {
        return String(names[idx]);
    }
    throw StrconvError("Failed to convert enum to string: invalid enum value");
}

/// @brief Converts a string to its corresponding enum value
/// @tparam Enum The enum type
/// @param s The string view to convert
/// @return The corresponding enum value
/// @throws StrconvError if the string does not match any name
template <typename Enum>
Enum enumFromString(StringView s) {
    auto names = EnumStrings<Enum>::names();
    auto it = std::find(names.begin(), names.end(), s);
    if (it != names.end()) {
        return static_cast<Enum>(std::distance(names.begin(), it));
    }
    throw StrconvError("Failed to convert string to enum: '" + String(s) + "'");
}

} // namespace cx

/// @brief Macro to define string representations for an enum type.
///
/// This macro should be used in a .cpp file to provide the string names for a
/// specific enum type. It also handles automatic registration of the enum.
///
/// @param Type The enum type
/// @param ... Variadic list of string literals
#define TD_DEF_ENUM_STRINGS(Type, ...)                                                   \
    template <>                                                                          \
    struct cx::EnumStrings<Type> {                                                       \
        static const Slice<StringView> names() {                                         \
            (void) _autoregister;                                                        \
            static StringView names[] = { __VA_ARGS__ };                                 \
            return Slice(names);                                                         \
        }                                                                                \
    private:                                                                             \
        static inline cx::EnumStringsAutoRegister<Type> _autoregister = {};              \
    };
