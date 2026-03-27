#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <stdexcept>
#include <cx-engine/defs/types.hpp>

namespace cx {

class GlobalSpriteAtlas;
class GlobalSpriteID {
private:
    usize value;

    friend class GlobalSpriteAtlas;
    explicit GlobalSpriteID(usize value) : value(value) {}
};

class GlobalSpriteAtlasError: public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class GlobalSpriteAtlas {
protected:
    Vec<sf::Sprite> sprites;

    friend class GlobalSpriteID;

public:
    template <typename... Args>
    GlobalSpriteID addSprite(Args&&... args) {
        sprites.emplace_back(std::forward<Args>(args)...);
        return GlobalSpriteID(sprites.size() - 1);
    }

    sf::Sprite& getSprite(GlobalSpriteID id) {
        if (id.value >= sprites.size()) {
            throw GlobalSpriteAtlasError("Invalid ID");
        }
        return sprites[id.value];
    }

    const sf::Sprite& getSprite(GlobalSpriteID id) const {
        if (id.value >= sprites.size()) {
            throw GlobalSpriteAtlasError("Invalid ID");
        }
        return sprites[id.value];
    }
};

}
