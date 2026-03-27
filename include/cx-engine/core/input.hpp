#include <cx-engine/defs/types.hpp>

namespace cx {

class Gamepad;

struct MouseAndKeyboard {};
using InputDevice = Union<MouseAndKeyboard, Gamepad*>;

}
