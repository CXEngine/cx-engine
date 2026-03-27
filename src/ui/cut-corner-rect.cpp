#include <cx-engine/ui/cut-corner-rect.hpp>

namespace cx {

sf::ConvexShape createCutCornerRect(float width, float height, float cut, float inset) {
    sf::ConvexShape shape(6);

    shape.setPoint(0, { cut + inset, inset });
    shape.setPoint(1, { width - inset, inset });
    shape.setPoint(2, { width - inset, height - cut - inset });
    shape.setPoint(3, { width - cut - inset, height - inset });
    shape.setPoint(4, { inset, height - inset });
    shape.setPoint(5, { inset, cut + inset });

    return shape;
}

}
