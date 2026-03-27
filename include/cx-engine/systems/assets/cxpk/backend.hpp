#pragma once

#include <cx-engine/defs/platform.hpp>

#define BACKEND_MEM   1
#define BACKEND_POSIX 2
#define BACKEND_WIN   3

#ifndef CXPK_BACKEND
#   if defined(_WIN32)
#       define CXPK_BACKEND BACKEND_WIN
#   elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)
#       define CXPK_BACKEND BACKEND_POSIX
#   else
#       define CXPK_BACKEND BACKEND_MEM
#   endif
#endif
