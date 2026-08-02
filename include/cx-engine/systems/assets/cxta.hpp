#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/fs.hpp>

#include <SFML/Graphics.hpp>

namespace cx {

CX_DEFINE_STANDARD_EXCEPTION(TextureAtlasLoadError, ResourceException);

class TextureAtlas {
public:
    TextureAtlas() = default;

    TextureAtlas(Slice<const byte> data) { load(data); }
    TextureAtlas(fs::path& file)         { open(file); }

    void load(Slice<const byte> data);
    void open(const fs::path& file);

    sf::IntRect getRect(const String& name) const;
    sf::Sprite getSprite(const String& name) const;
    const sf::Texture& getTexture() const;

private:
    static u64 xxh64Hash(const String& name);

    sf::Texture atlasTexture;
    HashMap<u64, sf::IntRect> entries;
};

} // namespace cx
