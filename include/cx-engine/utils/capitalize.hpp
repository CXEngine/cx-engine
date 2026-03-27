#include <cctype>
#include <cx-engine/defs/types.hpp>

namespace cx {

inline String capitalize(const String& s) {
    String r(s);
    r[0] = toupper(s[0]);
    return r;
}

}
