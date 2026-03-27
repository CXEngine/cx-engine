#include <cx-engine/systems/collision.hpp>
#include <cmath>

namespace cx {

static float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

void project(const Polygon& p, const sf::Vector2f& axis, float& min, float& max) {
    min = dot(p.points[0], axis);
    max = min;
    for (size_t i = 1; i < p.points.size(); ++i) {
        float projection = dot(p.points[i], axis);
        if (projection < min) {
            min = projection;
        }
        if (projection > max) {
            max = projection;
        }
    }
}

bool intervalOverlap(float minA, float maxA, float minB, float maxB) {
    return maxA >= minB && maxB >= minA;
}

bool intersects(const Polygon& p1, const Polygon& p2) {
    for (const auto& axis : p1.axes) {
        float minA, maxA, minB, maxB;
        project(p1, axis, minA, maxA);
        project(p2, axis, minB, maxB);
        if (!intervalOverlap(minA, maxA, minB, maxB)) {
            return false; // Found a separating axis
        }
    }
    for (const auto& axis : p2.axes) {
        float minA, maxA, minB, maxB;
        project(p1, axis, minA, maxA);
        project(p2, axis, minB, maxB);
        if (!intervalOverlap(minA, maxA, minB, maxB)) {
            return false; // Found a separating axis
        }
    }
    return true; // No separating axis found
}

} // namespace cx
