#pragma once

#include <cstdarg>
#include <functional>
#include <string>

namespace revointernal {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

using LogFunc = std::function<void(LogLevel level, const std::string& message)>;

void setLogLevel(LogLevel level);
LogLevel getLogLevel();
void setLogFunc(LogFunc func);

void log(LogLevel level, const char* fmt, ...);

template<LogLevel Level>
inline void log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    log(Level, "%s", buffer);
}

#define LOG_TRACE(...) ::RevoMii::log(::RevoMii::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...) ::RevoMii::log(::RevoMii::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...) ::RevoMii::log(::RevoMii::LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...) ::RevoMii::log(::RevoMii::LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) ::RevoMii::log(::RevoMii::LogLevel::Error, __VA_ARGS__)
#define LOG_CRITICAL(...) ::RevoMii::log(::RevoMii::LogLevel::Critical, __VA_ARGS__)

} // namespace revomii::util
