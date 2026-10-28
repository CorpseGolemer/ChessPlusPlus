#pragma once
#include <source_location>
#include <string_view>

#include "Types.h"

enum class LogLevel : u8 {
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
};

enum class LogCategory : u8 {
    Core = 0,
    Gui = 1,
    System = 2,
};

namespace logger {
    /// \brief Sets the minimum severity level to be printed
    void setLogLevel(LogLevel level);

    /// \brief Main logging function
    void log(LogLevel level,
             LogCategory category,
             std::string_view message,
             std::source_location location = std::source_location::current());

    inline void debug(const std::string_view message,
                      const LogCategory category = LogCategory::Core,
                      const std::source_location location = std::source_location::current()) {
        log(LogLevel::Debug, category, message, location);
    }

    inline void info(const std::string_view message,
                     const LogCategory category = LogCategory::Core,
                     const std::source_location location = std::source_location::current()) {
        log(LogLevel::Info, category, message, location);
    }

    inline void warning(const std::string_view message,
                 const LogCategory category = LogCategory::Core,
                 const std::source_location location = std::source_location::current()) {
        log(LogLevel::Warn, category, message, location);
    }

    inline void error(const std::string_view message,
                 const LogCategory category = LogCategory::Core,
                 const std::source_location location = std::source_location::current()) {
        log(LogLevel::Error, category, message, location);
    }
}