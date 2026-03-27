#include <cx-engine/systems/assets/cxpk/backend.hpp>
#if CXPK_BACKEND == BACKEND_POSIX

#include <cx-engine/systems/assets/cxpk.hpp>
#include <cx-engine/systems/assets/cxpk/posix-backend.hpp>
#include <cx-engine/systems/assets/cxpk/header-parser.hpp>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
#include <span>

namespace cx {

CxpkBackendImpl::CxpkBackendImpl() noexcept
    : fd(-1), data(nullptr), fileSize(0), openFlag(false) {}

CxpkBackendImpl::~CxpkBackendImpl() noexcept {
    closeImpl();
}

void CxpkBackendImpl::closeImpl() noexcept {
    if (data && fileSize > 0) {
        munmap((void*) data, fileSize);
    }

    if (fd != -1) {
        ::close(fd);
    }

    fd = -1;
    data = nullptr;
    fileSize = 0;
    mem.clear();
    entries.clear();
    openFlag = false;
}

void CxpkBackendImpl::loadFromFile(const std::filesystem::path& path) {
    closeImpl();

    int localFd = ::open(path.c_str(), O_RDONLY);
    if (localFd == -1)
        throw CxpkLoadError("failed to open CXPK file");

    struct stat st;
    if (fstat(localFd, &st) == -1) {
        ::close(localFd);
        throw CxpkLoadError("failed to stat CXPK file");
    }

    if (st.st_size == 0) {
        ::close(localFd);
        throw CxpkLoadError("expected CXPK data, found empty file");
    }

    void* mapPtr = mmap(nullptr, static_cast<usize>(st.st_size), PROT_READ, MAP_SHARED, localFd, 0);
    if (mapPtr == MAP_FAILED) {
        ::close(localFd);
        throw CxpkLoadError("mmap failed for CXPK file");
    }

    fd = localFd;
    fileSize = static_cast<usize>(st.st_size);
    data = reinterpret_cast<const byte*>(mapPtr);

    parseCxpkHeader(data, fileSize, entries);
    openFlag = true;
}

bool CxpkBackendImpl::isOpen() const noexcept {
    return openFlag && data != nullptr && fileSize > 0;
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

    return Slice(data + e.offset, e.size);
}

} // namespace cx

#endif
