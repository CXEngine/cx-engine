#pragma once

#include <cx-engine/defs/types.hpp>

#include <sstream>
#include <exception>

namespace cx {

class Exception: public std::exception {
protected:
    String msg;
    SourceLoc loc;

public:
    Exception(StringView msg, SourceLoc loc = SourceLoc::current())
        : msg(msg.data(), msg.length())
        , loc(loc)
    {}

    const char* what() const noexcept override {
        return msg.data();
    }

    virtual StringView name() const noexcept = 0;

    virtual String toString() const noexcept {
        std::stringstream ss;
        ss << '[' << loc.file_name() << ':' << loc.column() << " in " << loc.function_name() << ']';
        ss << ' ' << name() << ": " << msg;
        return std::move(ss).str();
    }
};

#define CX_STANDARD_EXCEPTIONS_MAP(FUNC)            \
    FUNC(IndexOutOfRangeException, Exception);      \
    FUNC(KeyNotFoundException, Exception);          \
    FUNC(OverflowException, Exception);             \
    FUNC(ResourceException, Exception);             \
    FUNC(InvalidArgumentException, Exception);      \
    FUNC(EmptyCollectionException, Exception);      \
                                                    \
    FUNC(EngineException, Exception);               \
                                                    \
    FUNC(EngineGraphicsException, EngineException); \
    FUNC(EngineLogicException, EngineException);    \
    FUNC(EnginePhysicsException, EngineException);  \

#define CX_DEFINE_STANDARD_EXCEPTION(NAME, BASE) \
    class NAME: public BASE {                    \
    public:                                      \
        using BASE::BASE;                        \
                                                 \
        virtual StringView                       \
        name() const noexcept override {         \
            return #NAME;                        \
        }                                        \
    }
CX_STANDARD_EXCEPTIONS_MAP(CX_DEFINE_STANDARD_EXCEPTION);

}
