#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>
#include <cx-engine/core/entity/team.hpp>
#include <cx-engine/systems/assets/cxmp.hpp>
#include <cx-engine/utils/hybrid-ptr.hpp>
#include <cx-engine/core/world/object-pool.hpp>

namespace cx {

class World: public sf::Drawable {
public:
    HybridPtr<TileMap> map;
    ObjectPool objects;
    Vec<Team> teams;

    void update(float dt) {
        objects.update(dt);
    }

    void handle(const sf::Event& event) {
        objects.handle(event);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override {
        objects.draw(target, states);
    }
};

}
