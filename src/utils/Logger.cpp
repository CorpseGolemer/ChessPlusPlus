#include "Logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <format>

namespace logger {
    namespace {
        auto minLogLevel = LogLevel::Debug;
        std::ofstream logFile("log.txt", std::ios::trunc);

        constexpr std::string_view levelToString(const LogLevel level) {
            switch (level) {
                case LogLevel::Debug: return "[DEBUG]";
                case LogLevel::Info: return "[INFO]";
                case LogLevel::Warn: return "[WARNING]";
                case LogLevel::Error: return "[ERROR]";
            }
            return "[UNKNOWN]";
        }

        constexpr std::string_view categoryToString(const LogCategory category) {
            switch (category) {
                case LogCategory::Core: return "[Core]";
                case LogCategory::Gui: return "[Gui]";
                case LogCategory::System: return "[System]";
            }
            return "[Unknown]";
        }
    }

    void setLogLevel(const LogLevel level) {
        minLogLevel = level;
    }

    void log(const LogLevel level,
             const LogCategory category,
             const std::string_view message,
             const std::source_location location) {

        if (level < minLogLevel) return;

        const std::string formattedLog = std::format("{} {} [{}:{}] {}",
            levelToString(level),
            categoryToString(category),
            std::filesystem::path(location.file_name()).filename().string(),
            location.line(),
            message
        );

        std::cout << formattedLog << "\n";

        if (logFile.is_open()) {
            logFile << formattedLog << "\n";
            logFile.flush();
        }

    }
}
