#include <cx-engine/systems/logging/logger.hpp>

#include <iomanip>
#include <memory>
#include <iostream>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace cx {

namespace {
    std::unique_ptr<Logger> s_instance;
}

void Logger::init(const fs::path& logsDir) {
    s_instance = std::unique_ptr<Logger>(new Logger(logsDir));
}

Logger& Logger::get() {
    return *s_instance;
}

void Logger::log(
    LogLevel lvl,
    StringView msg,
    const LogData& data,
    const SourceLoc& location
) {
    get().doLog(lvl, msg, data, location);
}

void Logger::doLog(
    LogLevel lvl,
    StringView msg,
    const LogData& data,
    const SourceLoc& location
) {
    std::lock_guard<std::mutex> lock(logMutex);

    String timestamp = getTimestamp();
    String levelString = logLevelToString(lvl);

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("timestamp");
    writer.String(timestamp.c_str());
    writer.Key("level");
    writer.String(levelString.c_str());
    writer.Key("message");
    writer.String(msg.data(), static_cast<rapidjson::SizeType>(msg.length()));

    writer.Key("location");
    writer.StartObject();
    writer.Key("file");
    writer.String(location.file_name());
    writer.Key("line");
    writer.Uint(location.line());
    writer.Key("function");
    writer.String(location.function_name());
    writer.EndObject();

    if (data.size() > 0) {
        writer.Key("data");
        writer.StartObject();
        for (const auto& pair: data) {
            writer.Key(pair.first.data(), static_cast<rapidjson::SizeType>(pair.first.length()));
            writer.String(pair.second.c_str(), static_cast<rapidjson::SizeType>(pair.second.length()));
        }
        writer.EndObject();
    }
    writer.EndObject();
    logFileStream << s.GetString() << "\n";

    std::ostream& out = isCritical(lvl) ? std::cerr : std::cout;

    out << "[" << timestamp << "] [" << levelString << "] ["
              << location.file_name() << ":" << location.line() << " "
              << location.function_name() << "] " << msg;
    if (data.size() > 0) {
        out << " {";
        bool first = true;
        for (const auto& pair: data) {
            if (!first) {
                out << ", ";
            }
            out << pair.first << ": " << pair.second;
            first = false;
        }
        out << "}";
    }
    out << std::endl;
}

String Logger::logLevelToString(LogLevel lvl) const {
    switch (lvl) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO";
        case LogLevel::Warn:  return "WARN";
        case LogLevel::Error: return "ERROR";
        default:              return "UNKNOWN";
    }
}

bool Logger::isCritical(LogLevel lvl) const {
    if (lvl == LogLevel::Error || lvl == LogLevel::Warn) {
        return true;
    }
    return false;
}

String Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

void Logger::openLogFile() {
    if (!fs::exists(logsDirPath)) {
        fs::create_directories(logsDirPath);
    }

    if (logFileStream.is_open()) {
        logFileStream.close();
    }

    String filename = getTimestamp() + ".jsonl";
    currentLogFilePath = logsDirPath / filename;
    logFileStream.open(currentLogFilePath, std::ios_base::app);

    if (!logFileStream.is_open()) {
        std::cerr << "ERROR: Failed to open log file: " << currentLogFilePath << std::endl;
    }
}

Logger::Logger(const fs::path& logsDir)
    : logsDirPath(logsDir) {
    openLogFile();
}

Logger::~Logger() {
    if (logFileStream.is_open()) {
        logFileStream << std::flush;
        logFileStream.close();
    }
    std::cout << std::flush;
}

}
