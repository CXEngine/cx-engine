#pragma once

#if defined(__linux__) || defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__)) || defined(__CYGWIN__)
    #define IS_POSIX 1
#else
    #define IS_POSIX 0
#endif

#if defined(__APPLE__)
    #define IS_MACOS 1
#else
    #define IS_MACOS 0
#endif

#if defined(__linux__)
    #define IS_LINUX 1
#else
    #define IS_LINUX 0
#endif

#if defined(_WIN32)
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
#endif
