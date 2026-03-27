#pragma once

#include <cx-engine/defs/types.hpp>
#include <cx-engine/systems/assets/cxpk/error.hpp>
#include <cx-engine/systems/assets/cxpk/entry.hpp>
#include <cstring>

namespace cx {

static inline void parseCxpkHeader(const byte* base, usize totalSize, HashMap<StringView, CxpkEntry>& out) {
    if (totalSize < 8) throw CxpkLoadError("CXPK header truncated");

    usize idx = 0;
    constexpr char CXPK_MAGIC[] = { 'C', 'X', 'P', 'K' };
    constexpr usize CXPK_MAGIC_SIZE = 4;

    if (std::memcmp(base + idx, CXPK_MAGIC, CXPK_MAGIC_SIZE) != 0)
        throw CxpkLoadError("invalid CXPK magic");
    idx += CXPK_MAGIC_SIZE;

    u32 fileCount;
    std::memcpy(&fileCount, base + idx, 4);
    idx += 4;

    const usize entryHeaderSize = 64 + 4 + 4;
    if (totalSize < idx + (usize) fileCount * entryHeaderSize)
        throw CxpkLoadError("CXPK entry header truncated");

    out.clear();
    for (u32 i = 0; i < fileCount; ++i) {
        const char* namePtr = (const char*) (base + idx);
        usize nameLen = strnlen(namePtr, 64);
        idx += 64;

        u32 offset;
        u32 size;
        std::memcpy(&offset, base + idx, 4); idx += 4;
        std::memcpy(&size,   base + idx, 4); idx += 4;

        if ((usize) offset > totalSize || (usize) size > totalSize - (usize) offset)
           throw CxpkLoadError("found corrupted CXPK entry in header");

        out.emplace(StringView(namePtr, nameLen), CxpkEntry{offset, size});
    }
}

} // namespace cx
