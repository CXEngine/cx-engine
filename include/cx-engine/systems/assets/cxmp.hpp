#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics.hpp>

#include <filesystem>
#include <stdexcept>

namespace cx {

CX_DEFINE_STANDARD_EXCEPTION(TileMapLoadError, ResourceException);

#pragma pack(push, 1)
struct TileMapHeader {
    char magic[4];
    u16 version;
    u16 tileSize;
    u32 mapWidth;
    u32 mapHeight;
    u32 tilesX;
    u32 tilesY;
    u32 tileCount;
    u32 hitboxCount;
    u32 uvOffset;
    u32 pixelOffset;
    u32 tileTableOffset;
    u32 hitboxOffset;
};
struct TileUV { float u0, v0, u1, v1; };
struct TileTableEntry { u32 firstHitbox, hitboxCount; };
struct Hitbox { float x, y, w, h, rotation; };
#pragma pack(pop)

class TileMap {
private:
    TileMapHeader header{};
    Slice<const byte> mapData;
    mutable Vec<Optional<sf::Texture>> tileTextures;
    bool loaded = false;

    const byte* getRawTilePixels(u32 idx) const;

public:
    TileMap() = default;
    explicit TileMap(const std::filesystem::path& file);

    void load(Slice<const byte> data);

    u16 getVersion() const noexcept { return header.version; }
    u32 getTileSize() const noexcept { return header.tileSize; }
    u32 getMapWidth() const noexcept { return header.mapWidth; }
    u32 getMapHeight() const noexcept { return header.mapHeight; }
    u32 getTilesX() const noexcept { return header.tilesX; }
    u32 getTilesY() const noexcept { return header.tilesY; }
    u32 getTileCount() const noexcept { return header.tileCount; }
    u32 getHitboxCount() const noexcept { return header.hitboxCount; }

    sf::Texture& acquireTileTexture(u32 idx) const;
    void releaseTileTexture(u32 idx);
    bool isTileTextureLoaded(u32 idx) const;

    TileUV getTileUV(u32 idx) const;
    Vec<Hitbox> getHitboxesForTile(u32 tileIdx) const;
    bool isLoaded() const noexcept { return loaded; }
};

} // namespace cx
