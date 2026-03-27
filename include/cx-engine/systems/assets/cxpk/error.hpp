#pragma once

#include <stdexcept>

namespace cx {

class CxpkLoadError: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace cx
