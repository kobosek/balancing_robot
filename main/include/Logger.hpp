#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include <string>
#include <unordered_map>

class Logger {
public:
    enum class LogLevel {
        NONE = ESP_LOG_NONE,
        ERROR = ESP_LOG_ERROR,
        WARN = ESP_LOG_WARN,
        INFO = ESP_LOG_INFO,
        DEBUG = ESP_LOG_DEBUG,
        VERBOSE = ESP_LOG_VERBOSE
    };

    static void setGlobalLogLevel(LogLevel level);
    static void setLogLevel(const char* tag, LogLevel level);

    template<typename... Args>
    static void error(const char* tag, const char* format, Args... args) {
        log(ESP_LOG_ERROR, tag, format, args...);
    }

    template<typename... Args>
    static void warn(const char* tag, const char* format, Args... args) {
        log(ESP_LOG_WARN, tag, format, args...);
    }

    template<typename... Args>
    static void info(const char* tag, const char* format, Args... args) {
        log(ESP_LOG_INFO, tag, format, args...);
    }

    template<typename... Args>
    static void debug(const char* tag, const char* format, Args... args) {
        log(ESP_LOG_DEBUG, tag, format, args...);
    }

    template<typename... Args>
    static void verbose(const char* tag, const char* format, Args... args) {
        log(ESP_LOG_VERBOSE, tag, format, args...);
    }

private:
    static LogLevel globalLogLevel;
    static std::unordered_map<std::string, LogLevel> tagLevels;

    template<typename... Args>
    
    static void log(esp_log_level_t level, const char* tag, const char* format, Args... args) {
        if (shouldLog(level, tag)) {
            esp_log_write(level, tag, format, args...);
        }
    }

    static bool shouldLog(esp_log_level_t level, const char* tag);
};