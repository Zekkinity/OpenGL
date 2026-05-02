#pragma once

#include <filesystem>
#include <string>

#include <iostream>

namespace zk {

enum class LogLevel {
    Info,
    Debug,
    Error,
    Warn,
};

inline const char* ToString(LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Debug:
        return "DEBUG";
    }
    return "";
}

template <typename... Args>
void LogMessage(
    std::string_view source,
    LogLevel level,
    std::string_view fmt,
    const char* file,
    int line,
    const char* func,
    Args&&... args) {
    auto message = std::vformat(fmt, std::make_format_args(args...));

    if (level == LogLevel::Error) {
        std::string filename = std::filesystem::path(file).filename().string();
        std::cout << "[" << ToString(level) << "] "
                  << "[" << filename << ":" << line << "] " << message << "\n";
    }
    else {
        std::string filename =
            std::filesystem::path(file).replace_extension("").filename().string();
        std::cout << "[" << source << "] "
                  << "[" << filename << "] "
                  << "[" << ToString(level) << "] " << message << "\n";
    }
}

// Make macros for logging
#if defined(__clang__) || defined(__GNUC__)
#define ZK_FUNC __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define ZK_FUNC __FUNCSIG__
#else
#define ZK_FUNC __func__
#endif

// Macros for engine
#define ZK_CORE_INFO(msg, ...)                                                                     \
    zk::LogMessage("ENGINE", zk::LogLevel::Info, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_CORE_WARN(msg, ...)                                                                     \
    zk::LogMessage("ENGINE", zk::LogLevel::Warn, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_CORE_ERROR(msg, ...)                                                                    \
    zk::LogMessage("ENGINE", zk::LogLevel::Error, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_CORE_DEBUG(msg, ...)                                                                    \
    zk::LogMessage("ENGINE", zk::LogLevel::Debug, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

// Macros for application
#define ZK_INFO(msg, ...)                                                                          \
    zk::LogMessage("APP", zk::LogLevel::Info, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_WARN(msg, ...)                                                                          \
    zk::LogMessage("APP", zk::LogLevel::Warn, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_ERROR(msg, ...)                                                                         \
    zk::LogMessage("APP", zk::LogLevel::Error, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

#define ZK_DEBUG(msg, ...)                                                                         \
    zk::LogMessage("APP", zk::LogLevel::Debug, msg, __FILE__, __LINE__, ZK_FUNC, ##__VA_ARGS__)

} // namespace zk
