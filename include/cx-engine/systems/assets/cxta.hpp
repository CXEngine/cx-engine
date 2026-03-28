#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics.hpp>

#include <filesystem>
#include <stdexcept>

#include <cx-engine/extern/xxhash/xxhash.h>

namespace cx {

struct TextureAtlasLoadError: public Exception {
    using Exception::Exception;
};

class TextureAtlas {
public:
    TextureAtlas() = default;

    void load(Slice<const byte> data);

    void open(const std::filesystem::path& file);

    sf::IntRect getRect(const String& name) const;
    sf::Sprite getSprite(const String& name) const;
    const sf::Texture& getTexture() const;

private:
    static u64 xxh64Hash(const String& name);

    sf::Texture atlasTexture;
    HashMap<u64, sf::IntRect> entries;
};

} // namespace cx
