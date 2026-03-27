
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace cx {

inline sf::Vector2f normalize(sf::Vector2f v) {
    float lenSq = v.x * v.x + v.y * v.y;
    if (lenSq == 0.f) {
        return {0.f, 0.f};
    }

    float invLen = 1.f / std::sqrt(lenSq);
    return { v.x * invLen, v.y * invLen };
}

inline sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t) {
    t = std::max(0.f, std::min(1.f, t));
    return sf::Color(
        (byte) (a.r + (b.r - a.r) * t),
        (byte) (a.g + (b.g - a.g) * t),
        (byte) (a.b + (b.b - a.b) * t),
        (byte) (a.a + (b.a - a.a) * t)
    );
}

}
