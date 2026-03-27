#include <cx-engine/systems/storage-manager.hpp>
#include <cx-engine/core/app.hpp>

#include <filesystem>
#include <cstdlib>

#if IS_POSIX
#include <sys/types.h>
#include <pwd.h>
#endif

#if IS_WINDOWS
#include <KnownFolders.h>
#include <ShlObj.h>
#include <objbase.h>
#endif

namespace cx {

fs::path StorageManager::initConfigDir() {
    fs::path path;

#   if IS_WINDOWS
        PWSTR widePath = nullptr;
        if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &widePath) == S_OK) {
            path = fs::path(widePath) / "Titandrop";
            CoTaskMemFree(widePath);
        }
#   elif IS_MACOS
        const char* home = getenv("HOME");
        if (home == nullptr) {
            struct passwd* pw = getpwuid(getuid());
            if (pw) {
                home = pw->pw_dir;
            }
        }

        if (home != nullptr) {
            path = fs::path(home)
                 / "Library"
                 / "Application Support"
                 / "Titandrop";
        }
#   else
        const char* xdg = getenv("XDG_CONFIG_HOME");
        if (xdg != nullptr) {
            path = fs::path(xdg) / app.name;
        } else {
            const char* home = getenv("HOME");
            if (home) {
                path = fs::path(home) / ".config" / app.name;
            }
        }
#   endif

    if (path.empty()) {
        path = fs::path(".") / "config";
    }

    fs::create_directories(path);
    return path;
}

fs::path StorageManager::initSettingsDir() {
    fs::path path = initConfigDir() / "settings";
    fs::create_directories(path);
    return path;
}

fs::path StorageManager::initSavesDir() {
    fs::path path;

#   if IS_WINDOWS
        PWSTR widePath = nullptr;
        if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &widePath) == S_OK) {
            path = fs::path(widePath) / "Titandrop";
            CoTaskMemFree(widePath);
        }
#   elif IS_MACOS
        const char* home = getenv("HOME");
        if (!home) {
            if (passwd* pw = getpwuid(getuid())) {
                home = pw->pw_dir;
            }
        }

        if (home) {
            path = fs::path(home)
                 / "Library"
                 / "Application Support"
                 / "Titandrop";
        }
#   else
        const char* xdg = getenv("XDG_DATA_HOME");
        if (xdg) {
            path = fs::path(xdg) / app.name;
        } else {
            const char* home = getenv("HOME");
            if (home) {
                path = fs::path(home) / ".local" / "share" / app.name;
            }
        }
#   endif

    if (path.empty()) {
        path = fs::path(".") / "saves";
    }

    fs::create_directories(path);
    return path;
}

fs::path StorageManager::initCacheDir() {
    fs::path path;

#   if IS_WINDOWS
        PWSTR widePath = nullptr;
        if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &widePath) == S_OK) {
            path = fs::path(widePath) / "Titandrop" / "Cache";
            CoTaskMemFree(widePath);
        }
#   elif IS_MACOS
        const char* home = getenv("HOME");
        if (!home) {
            if (passwd* pw = getpwuid(getuid())) {
                home = pw->pw_dir;
            }
        }

        if (home) {
            path = fs::path(home)
                 / "Library"
                 / "Caches"
                 / "Titandrop";
        }
#   else
        const char* xdg = getenv("XDG_CACHE_HOME");
        if (xdg) {
            path = fs::path(xdg) / app.name;
        } else {
            const char* home = getenv("HOME");
            if (home) {
                path = fs::path(home) / ".cache" / app.name;
            }
        }
#   endif

    if (path.empty()) {
        path = fs::path(".") / "cache";
    }

    fs::create_directories(path);
    return path;
}

fs::path StorageManager::initLogsDir() {
    fs::path path = initCacheDir() / "logs";
    fs::create_directories(path);
    return path;
}

} // namespace cx
