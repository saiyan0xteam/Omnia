#include "Logger.h"
#include <iostream>

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::SUCCESS: return "SUCCESS";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &time_t);
#else
    localtime_r(&time_t, &timeInfo);
#endif
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeInfo);
    return buffer;
}

void Logger::SetConsoleColor(LogLevel level) {
    // ANSI Escape Codes (Works on Linux and Modern Windows 10+)
    switch (level) {
        case LogLevel::DEBUG:   std::cout << "\033[90m"; break; // Gray
        case LogLevel::SUCCESS: std::cout << "\033[92m"; break; // Green
        case LogLevel::INFO:    std::cout << "\033[97m"; break; // White
        case LogLevel::WARNING: std::cout << "\033[93m"; break; // Yellow
        case LogLevel::ERR:     std::cout << "\033[91m"; break; // Red
        case LogLevel::FATAL:   std::cout << "\033[41;97m"; break; // Red BG, White Text
        default:                std::cout << "\033[0m";
    }
}

void Logger::ResetConsoleColor() {
    std::cout << "\033[0m";
}

void Logger::Initialize([[maybe_unused]] std::string conTitle) {
    // No explicit allocation needed for console applications.
    // ANSI escape codes enablement (handled by modern terminals)
}

void Logger::Shutdown() {
    // No cleanup needed
}
