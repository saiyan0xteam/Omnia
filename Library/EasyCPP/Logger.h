// Logger.h, an EasyCPP Package
// Created by Saiyan0x
// https://github.com/saiyan0xteam/EasyCPP

#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <Windows.h>
#include <chrono>
#include <utility>

enum class LogLevel {
    DEBUG,
    SUCCESS,
    INFO,
    WARNING,
    ERR,
    FATAL
};

class Logger {
private:
    inline static bool consoleAllocated = false;
    inline static std::string WideToUtf8(const wchar_t* wide) {
        if (!wide) return "(null)";
        int requiredBytes = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        if (requiredBytes <= 0) return "<wide-conv-error>";
        std::string result(static_cast<size_t>(requiredBytes), '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), requiredBytes, nullptr, nullptr);
        if (!result.empty() && result.back() == '\0') result.pop_back();
        return result;
    }

    inline static void Append(std::stringstream& ss, const wchar_t* value) { ss << WideToUtf8(value); }
    inline static void Append(std::stringstream& ss, wchar_t* value) { Append(ss, const_cast<const wchar_t*>(value)); }
    inline static void Append(std::stringstream& ss, const std::wstring& value) { Append(ss, value.c_str()); }
    inline static void Append(std::stringstream& ss, std::wstring_view value) { Append(ss, std::wstring(value).c_str()); }

    template <typename T>
    inline static void Append(std::stringstream& ss, T&& value) {
        ss << std::forward<T>(value);
    }

    inline static std::string GetCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm timeInfo;
        localtime_s(&timeInfo, &time_t);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeInfo);
        return buffer;
    }
    inline static std::string LevelToString(LogLevel level) {
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
    inline static void SetConsoleColor(LogLevel level) {
        if (!consoleAllocated) return;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        switch (level) {
        case LogLevel::DEBUG:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Gray
            break;
        case LogLevel::SUCCESS:
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Green
            break;
        case LogLevel::INFO:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // White
            break;
        case LogLevel::WARNING:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
            break;
        case LogLevel::ERR:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // Red
            break;
        case LogLevel::FATAL:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED); // Red background
            break;
        default:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // White
        }
    }
    inline static void ResetConsoleColor() {
        if (!consoleAllocated) return;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
public:
    inline static void Initialize(std::string conTitle) {
        if (!consoleAllocated) {
            if (!AllocConsole()) return;
            SetConsoleTitleA(conTitle.c_str());
            FILE* fDummy;
            freopen_s(&fDummy, "CONOUT$", "w", stdout);
            freopen_s(&fDummy, "CONOUT$", "w", stderr);
            freopen_s(&fDummy, "CONIN$", "r", stdin);
            consoleAllocated = true;
            Success("Logger", "Console Initialized");
        }
    }
    inline static void Shutdown() {
        if (consoleAllocated) {
            FreeConsole();
            consoleAllocated = false;
        }
    }
    template<typename... Args>
    inline static void Log(LogLevel level, const std::string& module, Args&&... args) {
        if (!consoleAllocated) return;
        std::stringstream ss;
        (Append(ss, std::forward<Args>(args)), ...);
        std::string message = ss.str();
        std::string logEntry = "[" + GetCurrentTime() + "] [" + LevelToString(level) + "] [" + module + "] " + message;
        SetConsoleColor(level);
        std::cout << logEntry << std::endl;
        ResetConsoleColor();
    }
    template<typename... Args>
    inline static void Debug(const std::string& module, Args&&... args) {
        Log(LogLevel::DEBUG, module, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline static void Success(const std::string& module, Args&&... args) {
        Log(LogLevel::SUCCESS, module, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline static void Info(const std::string& module, Args&&... args) {
        Log(LogLevel::INFO, module, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline static void Warning(const std::string& module, Args&&... args) {
        Log(LogLevel::WARNING, module, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline static void Error(const std::string& module, Args&&... args) {
        Log(LogLevel::ERR, module, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline static void Fatal(const std::string& module, Args&&... args) {
        Log(LogLevel::FATAL, module, std::forward<Args>(args)...);
    }
};
