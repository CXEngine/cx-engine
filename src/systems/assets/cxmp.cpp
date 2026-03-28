#include <cx-engine/systems/assets/cxmp.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Exception.hpp>

#include <cstring>

namespace cx {

void TileMap::load(Slice<const byte> data) {
    tileTextures.clear();
    loaded = false;
    mapData = data;

    const usize total = mapData.size();
    if (total < sizeof(TileMapHeader)) throw TileMapLoadError("CXMP header truncated");

    std::memcpy(&header, mapData.data(), sizeof(TileMapHeader));
    if (std::memcmp(header.magic, "CXMP", 4) != 0)
        throw TileMapLoadError("invalid CXMP magic");

    if (header.tileSize == 0) throw TileMapLoadError("tileSize == 0");
    if (header.tilesX == 0 || header.tilesY == 0 || header.tileCount == 0)
        throw TileMapLoadError("invalid tiles dimensions");

    usize uvBytes = sizeof(TileUV) * (usize) header.tileCount;
    if ((usize)header.uvOffset + uvBytes > total)
        throw TileMapLoadError("UV array out of bounds");

    usize tilePixelSize = (usize) header.tileSize * (usize) header.tileSize * 4;
    usize allPixelsBytes = tilePixelSize * (usize) header.tileCount;
    if ((usize)header.pixelOffset + allPixelsBytes > total)
        throw TileMapLoadError("pixel data out of bounds");

    usize tileTableBytes = sizeof(TileTableEntry) * (usize) header.tileCount;
    if ((usize)header.tileTableOffset + tileTableBytes > total)
        throw TileMapLoadError("tile table out of bounds");

    usize hitboxBytes = sizeof(Hitbox) * (usize) header.hitboxCount;
    if ((usize)header.hitboxOffset + hitboxBytes > total)
        throw TileMapLoadError("hitbox array out of bounds");

    tileTextures.resize(header.tileCount);

    loaded = true;
}

const byte* TileMap::getRawTilePixels(u32 idx) const {
    if (idx >= header.tileCount)
        throw TileMapLoadError("tile index out of range");
    usize tilePixelSize = (usize) header.tileSize * (usize) header.tileSize * 4;
    return mapData.data() + header.pixelOffset + idx * tilePixelSize;
}

sf::Texture& TileMap::acquireTileTexture(u32 idx) const {
    if (idx >= header.tileCount)
        throw TileMapLoadError("tile index out of range");

    auto& optTex = tileTextures[idx];
    if (!optTex) {
        try {
            optTex.emplace(sf::Vector2u{header.tileSize, header.tileSize});
        } catch (const sf::Exception& e) {
            throw TileMapLoadError("failed to create sf::Texture for tile " + std::to_string(idx) + ": " + e.what());
        }

        const byte* tilePixels = getRawTilePixels(idx);
        optTex->update(tilePixels);
        optTex->setSmooth(false);
    }
    return *optTex;
}

void TileMap::releaseTileTexture(u32 idx) {
    if (idx >= header.tileCount)
        throw TileMapLoadError("tile index out of range");
    tileTextures[idx].reset();
}

bool TileMap::isTileTextureLoaded(u32 idx) const {
    if (idx >= header.tileCount)
        throw TileMapLoadError("tile index out of range");
    return tileTextures[idx].has_value();
}

TileUV TileMap::getTileUV(u32 idx) const {
    if (idx >= header.tileCount)
        throw TileMapLoadError("tile uv index out of range");

    const auto* uvs = reinterpret_cast<const TileUV*>(mapData.data() + header.uvOffset);
    return uvs[idx];
}

Vec<Hitbox> TileMap::getHitboxesForTile(u32 tileIdx) const {
    if (tileIdx >= header.tileCount) throw TileMapLoadError("tile table index out of range");

    const auto* tileTable = reinterpret_cast<const TileTableEntry*>(mapData.data() + header.tileTableOffset);
    const TileTableEntry& te = tileTable[tileIdx];

    if (te.firstHitbox + te.hitboxCount > header.hitboxCount)
        throw TileMapLoadError("hitbox index out of range for tile " + std::to_string(tileIdx));

    Vec<Hitbox> out;
    if (te.hitboxCount > 0) {
        out.reserve(te.hitboxCount);
        const auto* flatHitboxes = reinterpret_cast<const Hitbox*>(mapData.data() + header.hitboxOffset);
        for (u32 i = 0; i < te.hitboxCount; ++i) {
            out.push_back(flatHitboxes[te.firstHitbox + i]);
        }
    }
    return out;
}

} // namespace cx
