#include <cx-engine/core/entity/components/after-image.hpp>

#include <cx-engine/core/entity/entity.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace cx {

AfterImageComponent::AfterImageComponent(Entity& parent, float spawnInterval, float ghostLifetime)
    : EntityComponent(parent)
    , spawnInterval(spawnInterval)
    , ghostLifetime(ghostLifetime)
    , timeSinceLastSpawn(0.f)
    , ghostColor(255, 255, 255, 128)
    , updateCallback(nullptr)
    , spawnCallback(nullptr)
{}

void AfterImageComponent::update(float dt) {
    timeSinceLastSpawn += dt;
    if (timeSinceLastSpawn >= spawnInterval) {
        spawn();
        timeSinceLastSpawn = 0.f;
    }

    for (auto it = ghosts.begin(); it != ghosts.end(); ) {
        it->lifetime -= dt;
        if (it->lifetime <= 0.f) {
            it = ghosts.erase(it);
        } else {
            if (updateCallback) {
                updateCallback(*it, dt);
            } else {
                float alphaRatio = it->lifetime / it->totalLifetime;
                sf::Color color = it->targetColor;
                color.a = static_cast<u8>(std::max((it->initialAlpha * alphaRatio) - 0.2f, 0.f));
                it->object.setColor(color);
            }
            ++it;
        }
    }
}

void AfterImageComponent::drawBefore(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& ghost: ghosts) {
        target.draw(ghost.object, states);
    }
}

void AfterImageComponent::spawn() {
    Ghost ghost = {
        .object = parent,
        .lifetime = ghostLifetime,
        .totalLifetime = ghostLifetime,
        .initialAlpha = 0,
        .targetColor = ghostColor,
    };

    if (spawnCallback) {
        ghost.object.setColor(ghostColor);
        spawnCallback(ghost);
    } else {
        ghost.object.setColor(ghostColor);
    }

    ghost.targetColor = ghost.object.getColor();
    ghost.initialAlpha = ghost.targetColor.a;

    ghosts.push_back(std::move(ghost));
}

void AfterImageComponent::setSpawnInterval(float interval) {
    spawnInterval = interval;
}

void AfterImageComponent::setGhostLifetime(float lifetime) {
    ghostLifetime = lifetime;
}

void AfterImageComponent::setGhostColor(const sf::Color& color) {
    ghostColor = color;
}

void AfterImageComponent::setUpdateCallback(GhostUpdateCallback callback) {
    updateCallback = std::move(callback);
}

void AfterImageComponent::setSpawnCallback(GhostSpawnCallback callback) {
    spawnCallback = std::move(callback);
}

}
