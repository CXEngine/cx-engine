#include <cx-engine/defs/platform.hpp>
#if CX_PLATFORM_IS_WINDOWS

#include <cx-engine/systems/assets/cxpk.hpp>
#include "windows-backend.hpp"
#include <cx-engine/systems/assets/cxpk/header-parser.hpp>

#include <windows.h>
#include <vector>
#include <cstring>
#include <filesystem>

namespace cx {

CxpkBackendImpl::CxpkBackendImpl() noexcept
    : hFile(nullptr), hMap(nullptr), data(nullptr), fileSize(0), openFlag(false) {}

CxpkBackendImpl::~CxpkBackendImpl() noexcept {
    closeImpl();
}

void CxpkBackendImpl::closeImpl() noexcept {
    if (data && hMap) {
        UnmapViewOfFile(reinterpret_cast<LPCVOID>(data));
    }
    if (hMap) {
        CloseHandle(hMap);
    }
    if (hFile) {
        CloseHandle(hFile);
    }

    hFile = nullptr;
    hMap = nullptr;
    data = nullptr;
    fileSize = 0;
    mem.clear();
    entries.clear();
    openFlag = false;
}

void CxpkBackendImpl::loadFromFile(const std::filesystem::path& path) {
    closeImpl();

    std::wstring wpath = path.wstring();
    HANDLE fileHandle = CreateFileW(
        wpath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE || fileHandle == nullptr)
        throw CxpkLoadError("failed to open CXPK file");

    LARGE_INTEGER sizeLi;
    if (!GetFileSizeEx(fileHandle, &sizeLi) || sizeLi.QuadPart == 0) {
        CloseHandle(fileHandle);
        throw CxpkLoadError("failed to get file size or file is empty");
    }

    HANDLE mapHandle = CreateFileMappingW(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!mapHandle) {
        CloseHandle(fileHandle);
        throw CxpkLoadError("CreateFileMapping failed");
    }

    void* view = MapViewOfFile(mapHandle, FILE_MAP_READ, 0, 0, 0);
    if (!view) {
        CloseHandle(mapHandle);
        CloseHandle(fileHandle);
        throw CxpkLoadError("MapViewOfFile failed");
    }

    hFile = fileHandle;
    hMap = mapHandle;
    fileSize = static_cast<usize>(sizeLi.QuadPart);
    data = reinterpret_cast<const byte*>(view);

    ::parseCxpkHeader(data, fileSize, entries);
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
        throw CxpkLoadError("asset not found: " + path);

    const CxpkEntry& e = it->second;
    if (static_cast<usize>(e.offset) + static_cast<usize>(e.size) > fileSize)
        throw CxpkLoadError("corrupted CXPK entry: " + path);

    return Slice(data + e.offset, e.size);
}

} // namespace cx

#endif
