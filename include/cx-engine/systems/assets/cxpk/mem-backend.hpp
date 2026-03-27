#pragma once

#include <cx-engine/defs/types.hpp>
#include <cx-engine/systems/assets/cxpk/header-parser.hpp>
#include <cx-engine/systems/assets/cxpk/entry.hpp>

#include <filesystem>
#include <vector>

namespace cx {

struct CxpkBackendImpl {
    CxpkBackendImpl() noexcept = default;
    ~CxpkBackendImpl() noexcept = default;

    void loadFromFile(const std::filesystem::path& path);

    bool isOpen() const noexcept;

    bool has(StringView path) const;
    Slice<const byte> getBytes(StringView path) const;

private:
    Vec<byte> data;
    usize fileSize = 0;
    HashMap<StringView, CxpkEntry> entries;
    bool openFlag = false;
};

} // namespace cx
