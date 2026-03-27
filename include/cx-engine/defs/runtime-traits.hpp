#pragma once

#include <cx-engine/defs/types.hpp>

namespace cx {
namespace rtraits {

static inline bool isIntegral(std::type_index ti) {
    static const Set<TypeIndex> integralTypes = {
        typeid(bool),
        typeid(char),
        typeid(signed char),
        typeid(unsigned char),
        typeid(short),
        typeid(unsigned short),
        typeid(int),
        typeid(unsigned int),
        typeid(long),
        typeid(unsigned long),
        typeid(long long),
        typeid(unsigned long long),
        typeid(char8_t),
        typeid(char16_t),
        typeid(char32_t),
        typeid(wchar_t)
    };

    return integralTypes.contains(ti);
}

}
}
