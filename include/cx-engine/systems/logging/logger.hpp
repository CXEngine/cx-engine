#pragma once
#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/fs.hpp>

#include <cx-engine/utils/buf.hpp>

#include <fstream>
#include <mutex>

namespace cx {

using LogData = InitList<Pair<StringView, String>>;

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
};

class Logger {
private:
    fs::path logsDirPath;
    std::ofstream logFileStream;
    fs::path currentLogFilePath;
    std::mutex logMutex;

    String getTimestamp() const;
    void openLogFile();
    String logLevelToString(LogLevel lvl) const;
    bool isCritical(LogLevel lvl) const;

    Logger(const fs::path& logsDir);

    void doLog(
        LogLevel lvl,
        StringView msg,
        const LogData& data,
        const SourceLoc& location
    );

public:
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static void init(const fs::path& logsDir);
    static Logger& get();

    static void log(
        LogLevel lvl,
        StringView msg,
        const LogData& data = {},
        const SourceLoc& location = SourceLoc::current()
    );

    inline static void debug(StringView msg, const LogData& data = {}, const SourceLoc& location = SourceLoc::current()) {
#ifdef TD_DEBUG
        return log(LogLevel::Debug, msg, data, location);
#endif
    }

    inline static void info(StringView msg, const LogData& data = {}, const SourceLoc& location = SourceLoc::current()) {
        return log(LogLevel::Info, msg, data, location);
    }

    inline static void warn(StringView msg, const LogData& data = {}, const SourceLoc& location = SourceLoc::current()) {
        return log(LogLevel::Warn, msg, data, location);
    }

    inline static void err(StringView msg, const LogData& data = {}, const SourceLoc& location = SourceLoc::current()) {
        return log(LogLevel::Error, msg, data, location);
    }
};

}
