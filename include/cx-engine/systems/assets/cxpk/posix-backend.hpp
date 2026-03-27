#pragma once
#include <cx-engine/defs/platform.hpp>

#if IS_POSIX

#include <cx-engine/systems/assets/cxpk/entry.hpp>
#include <cx-engine/defs/types.hpp>

#include <filesystem>

namespace cx {

struct CxpkBackendImpl {
    CxpkBackendImpl() noexcept;
    ~CxpkBackendImpl() noexcept;

    void loadFromFile(const std::filesystem::path& path);

    bool isOpen() const noexcept;
    bool has(StringView path) const;
    Slice<const byte> getBytes(StringView path) const;

private:
    void closeImpl() noexcept;

    int fd = -1;
    const byte* data = nullptr;
    usize fileSize = 0;
    Vec<byte> mem;
    HashMap<StringView, CxpkEntry> entries;
    bool openFlag = false;
};

} // namespace cx

#else
    #error "This file should be included only on posix systems"
#endif
