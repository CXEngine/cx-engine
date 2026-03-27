#pragma once

#include <cx-engine/utils/enumstr.hpp>

namespace cx { enum class WindowType; }

enum class cx::WindowType {
    Normal,
    Splash,
};
TD_DEF_ENUM_STRINGS(cx::WindowType, "normal", "splash");
