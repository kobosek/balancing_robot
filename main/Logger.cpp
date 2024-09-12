#include "include/Logger.hpp"

Logger::LogLevel Logger::globalLogLevel = Logger::LogLevel::INFO;
std::unordered_map<std::string, Logger::LogLevel> Logger::componentLogLevels;

void Logger::setGlobalLogLevel(LogLevel level) {
    globalLogLevel = level;
}

void Logger::setComponentLogLevel(const std::string& component, LogLevel level) {
    componentLogLevels[component] = level;
}

Logger::LogLevel Logger::getEffectiveLogLevel(const char* tag) {
    auto it = componentLogLevels.find(tag);
    if (it != componentLogLevels.end()) {
        return it->second;
    }
    return globalLogLevel;
}