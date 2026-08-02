#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/fs.hpp>

#include <SFML/Graphics.hpp>

namespace cx {

/// Format:
/// - 4 bytes magic: "CXSI"
/// - u16: variant count
/// - for each variant:
///   - u16 width
///   - u16 height
///   - u32 png_data_len
///   - png_data (RGBA PNG)

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

    explicit ScaledImage(const fs::path& file)   { open(file); }
    explicit ScaledImage(Slice<const byte> data) { load(data); }
    explicit ScaledImage(sf::Texture texture)    { addVariant(std::move(texture)); }

    void addVariant(sf::Texture texture);

    void open(const fs::path& file);
    void load(Slice<const byte> data);

    usize getVariantCount() const noexcept { return variants.size(); }

    ScaledImageVariant getVariant(u16 index) const;
    sf::Sprite getSprite(u16 index) const;

    u16 chooseVariantIndexForSize(u32 spriteWidth, u32 spriteHeight) const;
    ScaledImageVariant chooseVariantForSize(u32 spriteWidth, u32 spriteHeight) const;
    const sf::Texture& chooseTextureForSize(u32 spriteWidth, u32 spriteHeight) const;
    sf::Sprite getSpriteForSize(u32 spriteWidth, u32 spriteHeight) const;
};

} // namespace cx
