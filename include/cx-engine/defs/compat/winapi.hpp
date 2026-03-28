#pragma once

#include <cx-engine/defs/platform.hpp>

#ifdef CX_PLATFORM_IS_WINDOWS
#  ifndef WIN32_LEAN_AND_MEAN 
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  undef DialogBox
#endif // CX_PLATFORM_IS_WINDOWS
