#pragma once

#include <cx-engine/utils/strconv.hpp>

#include <cx-engine/defs/types.hpp>

namespace cx {

template <typename Enum>
struct EnumStrings {
    static const Slice<StringView> names();
};

using EnumNamesFn = const Slice<StringView> (*)();
inline HashMap<TypeIndex, EnumNamesFn> enumStringsRegistry;

template <typename Enum>
inline void registerEnumStrings() {
    enumStringsRegistry[typeid(Enum)] = &EnumStrings<Enum>::names;
}

template <typename Enum>
struct EnumStringsAutoRegister {
    EnumStringsAutoRegister() {
        registerEnumStrings<Enum>();
    }
};

inline const Slice<StringView> getEnumStrings(const TypeIndex& type) {
    auto it = enumStringsRegistry.find(type);
    if (it == enumStringsRegistry.end()) {
        return Slice<StringView>();
    }
    return it->second();
}

template <typename Enum>
String enumToString(Enum value) {
    auto names = EnumStrings<Enum>::names();
    auto idx = static_cast<usize>(value);
    if (idx < names.size()) {
        return String(names[idx]);
    }
    throw StrconvError("Failed to convert enum to string: invalid enum value");
}

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

#define TD_DEF_ENUM_STRINGS(Type, ...)                                      \
    template <>                                                             \
    struct cx::EnumStrings<Type> {                                          \
        static const Slice<StringView> names() {                            \
            (void) _autoregister;                                           \
            static StringView names[] = { __VA_ARGS__ };                    \
            return Slice(names);                                            \
        }                                                                   \
    private:                                                                \
        static inline cx::EnumStringsAutoRegister<Type> _autoregister = {}; \
    };
