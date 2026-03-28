#pragma once

#include <cx-engine/defs/types.hpp>

#include <stdexcept>

namespace cx {

class Exception: public std::runtime_error {
public:
    Exception(StringView msg)
        : std::runtime_error(std::string(msg.data(), msg.length()))
    {}
};

class EngineException: public Exception {
    public:
        using Exception::Exception;
};

#define CX_STANDARD_EXCEPTIONS_MAP(FUNC)            \
    FUNC(IndexOutOfRangeException, Exception);      \
    FUNC(KeyNotFoundException, Exception);          \
    FUNC(OverflowException, Exception);             \
    FUNC(InvalidArgumentException, Exception);      \
    FUNC(EmptyCollectionException, Exception);      \
                                                    \
    FUNC(EngineGraphicsException, EngineException); \
    FUNC(EngineLogicException, EngineException);    \
    FUNC(EnginePhysicsException, EngineException);  \

#define _CX_DEFINE_STANDARD_EXCEPTION(NAME, BASE) \
    class NAME: public BASE {                     \
    public:                                       \
        using BASE::BASE;                         \
    }
CX_STANDARD_EXCEPTIONS_MAP(_CX_DEFINE_STANDARD_EXCEPTION);

}
