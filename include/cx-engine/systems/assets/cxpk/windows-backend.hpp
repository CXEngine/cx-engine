#pragma once
#include <cx-engine/defs/platform.hpp>

#if CX_PLATFORM_IS_WINDOWS

#include <cx-engine/defs/types.hpp>
#include <windows.h>
#include <vector>
#include <filesystem>
#include <cx-engine/systems/assets/cxpk/entry.hpp>

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

    HANDLE hFile = nullptr;
    HANDLE hMap = nullptr;
    const byte* data = nullptr;
    usize fileSize = 0;
    Vec<byte> mem;
    HashMap<StringView, CxpkEntry> entries;
    bool openFlag = false;
};

} // namespace cx

#else
    #error "windows-backend.hpp should only be included on Windows builds"
#endif
