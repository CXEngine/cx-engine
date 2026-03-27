#pragma once

#include <cx-engine/defs/types.hpp>

#include <SFML/System/Vector2.hpp>

namespace cx {

struct Polygon {
    Vec<sf::Vector2f> points;
    Vec<sf::Vector2f> axes;
};

void project(const Polygon& p, const sf::Vector2f& axis, float& min, float& max);
bool intervalOverlap(float minA, float maxA, float minB, float maxB);
bool intersects(const Polygon& p1, const Polygon& p2);

} // namespace cx
