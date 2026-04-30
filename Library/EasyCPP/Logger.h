// Logger.h, an EasyCPP Package
#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <chrono>
#include <utility>
#include <vector>

enum class LogLevel {
    DEBUG, SUCCESS, INFO, WARNING, ERR, FATAL
};

class Logger {
private:
    static std::string LevelToString(LogLevel level);
    static std::string GetCurrentTime();
    static void SetConsoleColor(LogLevel level);
    static void ResetConsoleColor();
    
    template <typename T>
    static void Append(std::stringstream& ss, T&& value) { ss << std::forward<T>(value); }

public:
    static void Initialize(std::string conTitle);
    static void Shutdown();

    template<typename... Args>
    static void Log(LogLevel level, const std::string& module, Args&&... args) {
        std::stringstream ss;
        (Append(ss, std::forward<Args>(args)), ...);
        std::string logEntry = "[" + GetCurrentTime() + "] [" + LevelToString(level) + "] [" + module + "] " + ss.str();
        SetConsoleColor(level);
        std::cout << logEntry << std::endl;
        ResetConsoleColor();
    }

    template<typename... Args> inline static void Debug(const std::string& m, Args&&... a) { Log(LogLevel::DEBUG, m, std::forward<Args>(a)...); }
    template<typename... Args> inline static void Success(const std::string& m, Args&&... a) { Log(LogLevel::SUCCESS, m, std::forward<Args>(a)...); }
    template<typename... Args> inline static void Info(const std::string& m, Args&&... a) { Log(LogLevel::INFO, m, std::forward<Args>(a)...); }
    template<typename... Args> inline static void Warning(const std::string& m, Args&&... a) { Log(LogLevel::WARNING, m, std::forward<Args>(a)...); }
    template<typename... Args> inline static void Error(const std::string& m, Args&&... a) { Log(LogLevel::ERR, m, std::forward<Args>(a)...); }
    template<typename... Args> inline static void Fatal(const std::string& m, Args&&... a) { Log(LogLevel::FATAL, m, std::forward<Args>(a)...); }
};
