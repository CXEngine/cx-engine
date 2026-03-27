#include <cx-engine/systems/assets/cxpk/backend.hpp>

#if CXPK_BACKEND == BACKEND_MEM

#include <cx-engine/systems/assets/cxpk.hpp>
#include <cx-engine/systems/assets/cxpk/mem-backend.hpp>
#include <cx-engine/systems/assets/cxpk/header-parser.hpp>

#include <fstream>
#include <span>

void CxpkBackendImpl::loadFromFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        throw CxpkLoadError("failed to open CXPK file");

    const std::streamsize size = file.tellg();
    if (size <= 0)
        throw CxpkLoadError("expected CXPK data, found empty file");

    file.seekg(0);
    data.resize(static_cast<usize>(size));
    if (!file.read(reinterpret_cast<char*>(data.data()), size))
        throw CxpkLoadError("failed to read CXPK file");

    fileSize = data.size();

    ::parseCxpkHeader(data.data(), fileSize, entries);
    openFlag = true;
}

bool CxpkBackendImpl::isOpen() const noexcept {
    return openFlag && fileSize > 0 && !data.empty();
}

bool CxpkBackendImpl::has(StringView path) const {
    return entries.find(path) != entries.end();
}

Slice<const byte> CxpkBackendImpl::getBytes(StringView path) const {
    if (!isOpen())
        throw CxpkLoadError("archive is not loaded");

    const auto it = entries.find(path);
    if (it == entries.end())
        throw CxpkLoadError("asset not found: " + String(path));

    const CxpkEntry& e = it->second;
    if (static_cast<usize>(e.offset) + static_cast<usize>(e.size) > fileSize)
        throw CxpkLoadError("corrupted CXPK entry: " + String(path));

    return Slice(data.data() + e.offset, e.size);
}

#endif
