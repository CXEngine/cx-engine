#pragma once

#include <cx-engine/core/entity/component.hpp>
#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace sf {
    class RenderTarget;
    class RenderStates;
}
namespace cx {
    class Entity;
    class RenderComponent;
    class TransformComponent;
}

namespace cx {

class AfterImageComponent: public EntityComponent {
public:
    struct Ghost {
        sf::Sprite sprite;
        float lifetime;
        float totalLifetime;
        u8 initialAlpha;
        sf::Color targetColor;
    };

    using GhostUpdateCallback = Func<void(Ghost& ghost, float deltaTime)>;
    using GhostSpawnCallback = Func<void(Ghost& ghost)>;

    AfterImageComponent(Entity& parent, float spawnInterval, float ghostLifetime);

    void update(float dt) override;
    void drawBefore(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;

    void setSpawnInterval(float interval);
    void setGhostLifetime(float lifetime);
    void setGhostColor(const sf::Color& color);
    void setUpdateCallback(GhostUpdateCallback callback);
    void setSpawnCallback(GhostSpawnCallback callback);

private:
    void spawn();

    Vec<Ghost> ghosts;
    float spawnInterval;
    float ghostLifetime;
    float timeSinceLastSpawn;
    sf::Color ghostColor;
    GhostUpdateCallback updateCallback;
    GhostSpawnCallback spawnCallback;
};

}
