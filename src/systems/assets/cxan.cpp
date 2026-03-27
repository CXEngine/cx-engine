#include <cx-engine/systems/assets/cxan.hpp>

#include <fstream>
#include <cstring>
#include <string>

namespace cx {

namespace {
    constexpr char CXAN_MAGIC[4] = { 'C', 'X', 'A', 'N' };
    constexpr char CXAP_MAGIC[4] = { 'C', 'X', 'A', 'P' };

    constexpr usize CXAN_HEADER_SIZE = 4 + 2 + 2 + 4 + 4 + 4;
    constexpr usize CXAP_HEADER_SIZE = 4 + 2 + 2 + 4 + 4 + 4;
}

void Animation::open(const std::filesystem::path& file) {
    std::ifstream ifs(file, std::ios::binary | std::ios::ate);
    if (!ifs)
        throw AnimationLoadError("failed to open CXAN file: " + file.string());

    usize size = ifs.tellg();
    ifs.seekg(0);

    if (size <= 0)
        throw AnimationLoadError("expected CXAN data, found empty file: " + file.string());

    Vec<byte> buffer(static_cast<usize>(size));
    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size))
        throw AnimationLoadError("failed to read CXAN file: " + file.string());

    load(Slice(buffer));
}

void Animation::load(Slice<const byte> data) {
    pages.clear();
    frameMap.clear();

    const usize total = static_cast<usize>(data.size());
    const byte* base = data.data();

    if (total < CXAN_HEADER_SIZE)
        throw AnimationLoadError("CXAN header turuncated");

    usize idx = 0;

    if (memcmp(base + idx, CXAN_MAGIC, 4) != 0)
        throw AnimationLoadError("Invalid CXAN magic");
    idx += 4;

    u16 pagesCount = 0;
    memcpy(&pagesCount, base + idx, sizeof(u16));
    idx += sizeof(u16);

    u16 frameCount = 0;
    memcpy(&frameCount, base + idx, sizeof(u16));
    idx += sizeof(u16);

    (void) (float) (fps = 0);
    memcpy(&fps, base + idx, sizeof(float));
    idx += sizeof(float);

    u32 pagesOffset = 0;
    memcpy(&pagesOffset, base + idx, sizeof(u32));
    idx += sizeof(u32);

    u32 frameMapOffset = 0;
    memcpy(&frameMapOffset, base + idx, sizeof(u32));
    idx += sizeof(u32);

    if (pagesOffset >= total)
        throw AnimationLoadError("CXAN pages offset out of bounds");
    if (frameMapOffset >= total)
        throw AnimationLoadError("CXAN frame map offset out of bounds");

    pages.clear();
    pages.reserve(pagesCount);

    usize pos = static_cast<usize>(pagesOffset);
    for (u16 p = 0; p < pagesCount; ++p) {
        if (pos + CXAN_HEADER_SIZE > total)
            throw AnimationLoadError("CXAP header truncated");

        if (memcmp(base + pos, CXAP_MAGIC, 4) != 0)
            throw AnimationLoadError("invalid CXAP magic at page " + std::to_string(p));
        pos += 4;

        u16 cols = 0;
        u16 rows = 0;
        memcpy(&cols, base + pos, sizeof(u16)); pos += sizeof(u16);
        memcpy(&rows, base + pos, sizeof(u16)); pos += sizeof(u16);

        u32 fw = 0;
        u32 fh = 0;
        memcpy(&fw, base + pos, sizeof(u32)); pos += sizeof(u32);
        memcpy(&fh, base + pos, sizeof(u32)); pos += sizeof(u32);

        u32 pngSize = 0;
        memcpy(&pngSize, base + pos, sizeof(u32)); pos += sizeof(u32);

        if (pos + pngSize > total)
            throw AnimationLoadError("CXAP PNG data out of bounds");

        const void* pngPtr = base + pos;

        sf::Texture tex;
        tex.setSmooth(true);
        if (!tex.loadFromMemory(pngPtr, pngSize))
            throw AnimationLoadError("Failed to load CXAP PNG for page " + std::to_string(p));

        AnimationPage page;
        page.texture = std::move(tex);
        page.cols = cols;
        page.rows = rows;
        page.frameWidth = static_cast<decltype(page.frameWidth)>(fw);
        page.frameHeight = static_cast<decltype(page.frameHeight)>(fh);

        pages.push_back(std::move(page));

        pos += pngSize;
    }

    const usize frameMapSizeBytes = static_cast<usize>(frameCount) * sizeof(u16) * 2;
    if ((usize) frameMapOffset + frameMapSizeBytes > total)
        throw AnimationLoadError("CXAN frame map out of bounds");

    frameMap.resize(static_cast<usize>(frameCount) * 2);
    usize fmPos = static_cast<usize>(frameMapOffset);
    for (usize i = 0; i < static_cast<usize>(frameCount); ++i) {
        u16 pageIndex = 0;
        u16 cellIndex = 0;
        memcpy(&pageIndex, base + fmPos, sizeof(u16)); fmPos += sizeof(u16);
        memcpy(&cellIndex, base + fmPos, sizeof(u16)); fmPos += sizeof(u16);

        frameMap[i * 2] = { pageIndex, cellIndex };
    }
}

const AnimationPage& Animation::getPage(u16 pageIndex) const {
    if (pageIndex >= pages.size())
        throw AnimationLoadError("CXAN page index out of the range");
    return pages[pageIndex];
}

AnimationFrame Animation::getFrame(u16 index) const {
    const u16 totalFrames = getFrameCount();
    if (index >= totalFrames)
        throw AnimationLoadError("Frame index out of range");

    const usize baseIdx = (usize) index * 2;
    auto [pageIndex, cellIndex] = frameMap[baseIdx + 0];

    if (pageIndex >= pages.size())
        throw AnimationLoadError("Frame references invalid page index");

    const AnimationPage& page = pages[pageIndex];

    const u32 cols = page.cols;
    const u32 rows = page.rows;
    const u32 fw = page.frameWidth;
    const u32 fh = page.frameHeight;

    if (cols == 0 || rows == 0)
        throw AnimationLoadError("Invalid page dimensions (cols/rows == 0)");

    const u32 cells = cols * rows;
    if (cellIndex >= cells)
        throw AnimationLoadError("Frame references invalid cell index");

    const u32 col = cellIndex % cols;
    const u32 row = cellIndex / cols;

    const int left = (int) (col * fw);
    const int top  = (int) (row * fh);
    const int w    = (int) (fw);
    const int h    = (int) (fh);

    sf::Texture& textureRef = const_cast<sf::Texture&>(page.texture);

    AnimationFrame out{ textureRef, sf::IntRect({ left, top }, { w, h }) };
    return out;
}

sf::Sprite Animation::getSprite(u16 index) const {
    AnimationFrame f = getFrame(index);
    sf::Sprite s(f.texture, f.rect);
    s.setPosition(sf::Vector2f(0.f, 0.f));
    return s;
}

} // namespace cx
