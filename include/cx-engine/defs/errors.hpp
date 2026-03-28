#pragma once

#include <SFML/System/Exception.hpp>

#include <stdexcept>

namespace cx {

class Exception: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class GraphicsException: public Exception {
public:
    using Exception::Exception;
};
class LogicException: public Exception {
public:
    using Exception::Exception;
};
class PhysicsException: public Exception {
public:
    using Exception::Exception;
};

}
