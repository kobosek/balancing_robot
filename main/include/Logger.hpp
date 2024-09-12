#pragma once
#include "esp_err.h"
#include "esp_log.h"
#include <string>
#include <unordered_map>

class Logger {
public:
    enum class LogLevel {
        ERROR,
        WARN,
        INFO,
        DEBUG,
        VERBOSE
    };

    static void setGlobalLogLevel(LogLevel level);
    static void setComponentLogLevel(const std::string& component, LogLevel level);

    template<typename... Args>
    static void log(const char* tag, LogLevel level, const char* format, Args... args) {
        if (level > getEffectiveLogLevel(tag)) return;

        switch (level) {
            case LogLevel::ERROR:
                ESP_LOGE(tag, format, args...);
                break;
            case LogLevel::WARN:
                ESP_LOGW(tag, format, args...);
                break;
            case LogLevel::INFO:
                ESP_LOGI(tag, format, args...);
                break;
            case LogLevel::DEBUG:
                ESP_LOGD(tag, format, args...);
                break;
            case LogLevel::VERBOSE:
                ESP_LOGV(tag, format, args...);
                break;
        }
    }

    template<typename... Args>
    static void error(const char* tag, const char* format, Args... args) {
        log(tag, LogLevel::ERROR, format, args...);
    }

    template<typename... Args>
    static void warn(const char* tag, const char* format, Args... args) {
        log(tag, LogLevel::WARN, format, args...);
    }

    template<typename... Args>
    static void info(const char* tag, const char* format, Args... args) {
        log(tag, LogLevel::INFO, format, args...);
    }

    template<typename... Args>
    static void debug(const char* tag, const char* format, Args... args) {
        log(tag, LogLevel::DEBUG, format, args...);
    }

    template<typename... Args>
    static void verbose(const char* tag, const char* format, Args... args) {
        log(tag, LogLevel::VERBOSE, format, args...);
    }

private:
    static LogLevel globalLogLevel;
    static std::unordered_map<std::string, LogLevel> componentLogLevels;
    static LogLevel getEffectiveLogLevel(const char* tag);
};