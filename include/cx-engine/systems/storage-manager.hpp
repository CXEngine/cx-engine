#pragma once

#include <cx-engine/defs/types.hpp>
#include <cx-engine/utils/lazy.hpp>

#include <cx-engine/defs/fs.hpp>
#include <functional>

namespace cx {

class App;

class StorageManager {
protected:
    App& app;

    Lazy<fs::path> configDir;
    Lazy<fs::path> settingsDir;
    Lazy<fs::path> savesDir;
    Lazy<fs::path> logsDir;
    Lazy<fs::path> cacheDir;

    fs::path initConfigDir();
    fs::path initSettingsDir();

    fs::path initSavesDir();
    fs::path initCacheDir();
    fs::path initLogsDir();

public:
    StorageManager(App& app)
        : app(app)
        , configDir(std::bind(&StorageManager::initConfigDir, this))
        , settingsDir(std::bind(&StorageManager::initSettingsDir, this))
        , savesDir(std::bind(&StorageManager::initSavesDir, this))
        , logsDir(std::bind(&StorageManager::initLogsDir, this))
        , cacheDir(std::bind(&StorageManager::initCacheDir, this))
    {}

    fs::path getConfigDir() { return configDir.get(); }
    fs::path getSettingsDir() { return settingsDir.get(); }
    fs::path getSavesDir() { return savesDir.get(); }
    fs::path getCacheDir() { return cacheDir.get(); }
    fs::path getLogsDir() { return logsDir.get(); }
};

} // namespace cx
