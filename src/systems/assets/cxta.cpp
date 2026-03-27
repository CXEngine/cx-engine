#include <cx-engine/systems/assets/cxta.hpp>

#include <cx-engine/defs/types.hpp>

#include <fstream>
#include <cstring>

namespace cx {

namespace {
    constexpr char CXTA_MAGIC[4] = { 'C', 'X', 'T', 'A' };
    constexpr u64 XXH_SEED = 0;
}

u64 TextureAtlas::xxh64Hash(const String& name) {
    return XXH64(name.data(), name.length(), XXH_SEED);
}

void TextureAtlas::open(const std::filesystem::path& file) {
    std::ifstream ifs(file, std::ios::binary | std::ios::ate);
    if (!ifs) {
        throw TextureAtlasLoadError("failed to open CXTA file: " + file.string());
    }

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (size <= 0) {
        throw TextureAtlasLoadError("CXTA file is empty: " + file.string());
    }

    Vec<byte> buffer(size);
    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw TextureAtlasLoadError("failed to read CXTA file: " + file.string());
    }

    load(Slice(buffer));
}

void TextureAtlas::load(Slice<const byte> data) {
    entries.clear();

    const byte* base = data.data();
    usize total_size = data.size();
    usize pos = 0;

    if (total_size < 12) {
        throw TextureAtlasLoadError("CXTA header truncated");
    }

    if (memcmp(base + pos, CXTA_MAGIC, 4) != 0) {
        throw TextureAtlasLoadError("Invalid CXTA magic");
    }
    pos += 4;

    u32 numEntries = 0;
    memcpy(&numEntries, base + pos, sizeof(u32));
    pos += sizeof(u32);

    u32 pngDataSize = 0;
    memcpy(&pngDataSize, base + pos, sizeof(u32));
    pos += sizeof(u32);

    if (pos + pngDataSize > total_size) {
        throw TextureAtlasLoadError("CXTA PNG data out of bounds");
    }

    if (!atlasTexture.loadFromMemory(base + pos, pngDataSize)) {
        throw TextureAtlasLoadError("Failed to load CXTA atlas texture from memory");
    }
    atlasTexture.setSmooth(false);
    pos += pngDataSize;

    // Entry: hash (8) + x(2) + y(2) + w(2) + h(2) = 16 bytes
    const usize entrySize = 8 + 2 + 2 + 2 + 2;
    if (pos + (numEntries * entrySize) > total_size) {
        throw TextureAtlasLoadError("CXTA entries data out of bounds");
    }

    entries.reserve(numEntries);
    for (u32 i = 0; i < numEntries; ++i) {
        u64 hash = 0;
        memcpy(&hash, base + pos, sizeof(u64));
        pos += sizeof(u64);

        u16 x = 0, y = 0, w = 0, h = 0;
        memcpy(&x, base + pos, sizeof(u16)); pos += sizeof(u16);
        memcpy(&y, base + pos, sizeof(u16)); pos += sizeof(u16);
        memcpy(&w, base + pos, sizeof(u16)); pos += sizeof(u16);
        memcpy(&h, base + pos, sizeof(u16)); pos += sizeof(u16);

        entries[hash] = sf::IntRect(
            { (int) x, (int) y },
            { (int) w, (int) h }
        );
    }
}

sf::IntRect TextureAtlas::getRect(const String& name) const {
    u64 hash = xxh64Hash(name);
    auto it = entries.find(hash);
    if (it == entries.end()) {
        throw TextureAtlasLoadError("CXTA entry not found: " + name);
    }
    return it->second;
}

sf::Sprite TextureAtlas::getSprite(const String& name) const {
    sf::IntRect rect = getRect(name);
    return sf::Sprite(atlasTexture, rect);
}

const sf::Texture& TextureAtlas::getTexture() const {
    return atlasTexture;
}

} // namespace cx
