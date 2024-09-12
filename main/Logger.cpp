#include "include/Logger.hpp"

Logger::LogLevel Logger::globalLogLevel = Logger::LogLevel::INFO;
std::unordered_map<std::string, Logger::LogLevel> Logger::tagLevels;

void Logger::setGlobalLogLevel(LogLevel level) {
    globalLogLevel = level;
    esp_log_level_set("*", static_cast<esp_log_level_t>(level));
}

void Logger::setLogLevel(const char* tag, LogLevel level) {
    tagLevels[tag] = level;
    esp_log_level_set(tag, static_cast<esp_log_level_t>(level));
}

bool Logger::shouldLog(esp_log_level_t level, const char* tag) {
    auto it = tagLevels.find(tag);
    if (it != tagLevels.end()) {
        return level <= static_cast<esp_log_level_t>(it->second);
    }
    return level <= static_cast<esp_log_level_t>(globalLogLevel);
}