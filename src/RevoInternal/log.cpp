#include "log.hpp"
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>

namespace revointernal {

namespace {
LogLevel logLevel = LogLevel::Info;
LogFunc logFunc = nullptr;
std::mutex logMutex;

const char* ANSI_RESET = "\033[0m";
const char* ANSI_TRACE = "\033[90m";      // Bright black (gray)
const char* ANSI_DEBUG = "\033[36m";      // Cyan
const char* ANSI_INFO = "\033[32m";       // Green
const char* ANSI_WARNING = "\033[33m";    // Yellow
const char* ANSI_ERROR = "\033[31m";      // Red
const char* ANSI_CRITICAL = "\033[1;31m"; // Bold red

const char* getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return ANSI_TRACE;
        case LogLevel::Debug: return ANSI_DEBUG;
        case LogLevel::Info: return ANSI_INFO;
        case LogLevel::Warning: return ANSI_WARNING;
        case LogLevel::Error: return ANSI_ERROR;
        case LogLevel::Critical: return ANSI_CRITICAL;
        default: return ANSI_RESET;
    }
}

const char* getLevelName(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}
}

void setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    logLevel = level;
}

LogLevel getLogLevel() {
    std::lock_guard<std::mutex> lock(logMutex);
    return logLevel;
}

void setLogFunc(LogFunc func) {
    std::lock_guard<std::mutex> lock(logMutex);
    logFunc = func;
}

void log(LogLevel level, const char* fmt, ...) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (level < logLevel) {
        return;
    }

    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::string message(buffer);

    if (logFunc) {
        logFunc(level, message);
    } else {
        const char* color = getColorCode(level);
        const char* levelName = getLevelName(level);
        std::cout << color << "[" << levelName << "] " << message << ANSI_RESET << std::endl;
    }
}

} // namespace revointernal
