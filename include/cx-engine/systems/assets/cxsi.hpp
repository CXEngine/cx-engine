#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics.hpp>

#include <filesystem>
#include <stdexcept>


namespace cx {

/*
CXSI — Simple image set format

Format:
- 4 bytes magic: "CXSI"
- u16: variant count
- for each variant:
  - u16 width
  - u16 height
  - u32 png_data_len
  - png_data (RGBA PNG)
*/

CX_DEFINE_STANDARD_EXCEPTION(ScaledImageLoadError, ResourceException);

// A single variant inside a CXSI file. The variant refers to an entire texture
// (no sub-rect needed)
struct ScaledImageVariant {
    const sf::Texture& texture;
    u16 width = 0;
    u16 height = 0;
};

class ScaledImage {
private:
    Vec<sf::Texture> variants;
    Vec<Pair<u16, u16>> dims;

public:
    explicit ScaledImage() = default;

    explicit inline ScaledImage(const std::filesystem::path& file) { open(file); }
    explicit inline ScaledImage(Slice<const byte> data)            { load(data); }
    explicit inline ScaledImage(sf::Texture texture)               { addVariant(std::move(texture)); }

    void addVariant(sf::Texture texture);

    void open(const std::filesystem::path& file);
    void load(Slice<const byte> data);

    inline usize getVariantCount() const noexcept { return variants.size(); }

    ScaledImageVariant getVariant(u16 index) const;
    sf::Sprite getSprite(u16 index) const;

    u16 chooseVariantIndexForSize(u32 spriteWidth, u32 spriteHeight) const;
    ScaledImageVariant chooseVariantForSize(u32 spriteWidth, u32 spriteHeight) const;
    const sf::Texture& chooseTextureForSize(u32 spriteWidth, u32 spriteHeight) const;
    sf::Sprite getSpriteForSize(u32 spriteWidth, u32 spriteHeight) const;
};

} // namespace cx
