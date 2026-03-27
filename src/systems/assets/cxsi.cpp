#include <cx-engine/systems/assets/cxsi.hpp>

#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <limits>

namespace cx {

namespace {
    constexpr char CXSI_MAGIC[4] = { 'C', 'X', 'S', 'I' };
}

void ScaledImage::open(const std::filesystem::path& file) {
    std::ifstream ifs(file, std::ios::binary | std::ios::ate);
    if (!ifs)
        throw CxsiLoadError("failed to open CXSI file: " + file.string());

    usize size = ifs.tellg();
    if (size <= 0)
        throw CxsiLoadError("expected CXSI data, found empty file: " + file.string());

    ifs.seekg(0);

    Vec<byte> buffer(static_cast<usize>(size));
    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size))
        throw CxsiLoadError("failed to read CXSI file: " + file.string());

    load(Slice<const byte>(buffer));
}

void ScaledImage::load(Slice<const byte> data) {
    variants.clear();
    dims.clear();

    const usize total = static_cast<usize>(data.size());
    const byte* base = data.data();

    if (total < 6) // magic (4) + variant count (2)
        throw CxsiLoadError("CXSI header truncated");

    usize idx = 0;

    if (memcmp(base + idx, CXSI_MAGIC, 4) != 0)
        throw CxsiLoadError("Invalid CXSI magic");
    idx += 4;

    u16 variantCount = 0;
    memcpy(&variantCount, base + idx, sizeof(u16));
    idx += sizeof(u16);

    if (variantCount == 0) return;

    variants.reserve(variantCount);
    dims.reserve(variantCount);

    for (u16 i = 0; i < variantCount; ++i) {
        if (idx + sizeof(u16) + sizeof(u16) + sizeof(u32) > total)
            throw CxsiLoadError("CXSI variant header truncated");

        u16 w = 0;
        u16 h = 0;
        u32 pngSize = 0;

        memcpy(&w, base + idx, sizeof(u16)); idx += sizeof(u16);
        memcpy(&h, base + idx, sizeof(u16)); idx += sizeof(u16);
        memcpy(&pngSize, base + idx, sizeof(u32)); idx += sizeof(u32);

        if (idx + static_cast<usize>(pngSize) > total)
            throw CxsiLoadError("CXSI PNG data out of bounds");

        const void* pngPtr = base + idx;

        sf::Texture tex;
        tex.setSmooth(false);
        if (!tex.loadFromMemory(pngPtr, static_cast<usize>(pngSize)))
            throw CxsiLoadError("Failed to load CXSI PNG for variant " + std::to_string(i));

        variants.push_back(std::move(tex));
        dims.push_back({ w, h });

        idx += static_cast<usize>(pngSize);
    }
}

CxsiVariant ScaledImage::getVariant(u16 index) const {
    if (index >= variants.size())
        throw CxsiLoadError("Variant index out of range");

    const sf::Texture& tref = variants[index];
    return CxsiVariant{ tref, dims[index].first, dims[index].second };
}

sf::Sprite ScaledImage::getSprite(u16 index) const {
    CxsiVariant v = getVariant(index);
    sf::Sprite s(v.texture);
    s.setPosition(sf::Vector2f(0.f, 0.f));
    return s;
}

u16 ScaledImage::chooseVariantIndexForSize(u32 spriteWidth, u32 spriteHeight) const {
    if (getVariantCount() == 0)
        throw CxsiLoadError("ScaledImage has no variants");

    if (spriteWidth == 0 || spriteHeight == 0)
        throw CxsiLoadError("Invalid sprite size");

    auto computeScale = [](u32 reqW, u32 reqH, u16 w, u16 h) -> double {
        double fx = static_cast<double>(reqW) / static_cast<double>(w);
        double fy = static_cast<double>(reqH) / static_cast<double>(h);
        return std::max(fx, fy);
    };

    // first pass: prefer variants that do not require upscaling (scale <= 1),
    // choosing the one with scale closest to 1 (i.e. maximal scale).
    bool found = false;
    double bestScale = -1.0;
    usize bestIndex = 0;
    double bestAreaDiff = 0.0;

    for (usize i = 0; i < variants.size(); ++i) {
        u16 w = dims[i].first;
        u16 h = dims[i].second;
        if (w == 0 || h == 0) continue;
        double s = computeScale(spriteWidth, spriteHeight, w, h);
        double areaDiff = std::abs(static_cast<double>(w) - static_cast<double>(spriteWidth))
                        + std::abs(static_cast<double>(h) - static_cast<double>(spriteHeight));
        if (s <= 1.0) {
            if (!found || s > bestScale || (s == bestScale && areaDiff < bestAreaDiff)) {
                found = true;
                bestScale = s;
                bestIndex = i;
                bestAreaDiff = areaDiff;
            }
        }
    }

    if (found) return static_cast<u16>(bestIndex);

    double bestScaleUp = std::numeric_limits<double>::infinity();
    bestIndex = 0;
    bestAreaDiff = 0.0;
    for (usize i = 0; i < variants.size(); ++i) {
        u16 w = dims[i].first;
        u16 h = dims[i].second;
        if (w == 0 || h == 0) continue;
        double s = computeScale(spriteWidth, spriteHeight, w, h);
        double areaDiff = std::abs(static_cast<double>(w) - static_cast<double>(spriteWidth))
                        + std::abs(static_cast<double>(h) - static_cast<double>(spriteHeight));
        if (s < bestScaleUp || (s == bestScaleUp && areaDiff < bestAreaDiff)) {
            bestScaleUp = s;
            bestIndex = i;
            bestAreaDiff = areaDiff;
        }
    }

    return static_cast<u16>(bestIndex);
}

CxsiVariant ScaledImage::chooseVariantForSize(u32 spriteWidth, u32 spriteHeight) const {
    return getVariant(chooseVariantIndexForSize(spriteWidth, spriteHeight));
}

const sf::Texture& ScaledImage::chooseTextureForSize(u32 spriteWidth, u32 spriteHeight) const {
    u16 idx = chooseVariantIndexForSize(spriteWidth, spriteHeight);
    return variants[static_cast<usize>(idx)];
}

sf::Sprite ScaledImage::getSpriteForSize(u32 spriteWidth, u32 spriteHeight) const {
    CxsiVariant v = chooseVariantForSize(spriteWidth, spriteHeight);
    sf::Sprite s(v.texture);

    if (v.width != 0 && v.height != 0) {
        float sx = static_cast<float>(spriteWidth) / static_cast<float>(v.width);
        float sy = static_cast<float>(spriteHeight) / static_cast<float>(v.height);
        s.setScale(sf::Vector2f(sx, sy));
    }

    s.setPosition(sf::Vector2f(0.f, 0.f));
    return s;
}

void ScaledImage::addVariant(sf::Texture texture) {
    sf::Vector2u ts = texture.getSize();
    dims.push_back({
        static_cast<u16>(ts.x),
        static_cast<u16>(ts.y),
    });

    variants.push_back(std::move(texture));
}

} // namespace cx
