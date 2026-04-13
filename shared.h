#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <raylib.h>
#include <raymath.h>
#include <EasyCPP/Logger.h>
#include <Windows.h>
#include <filesystem>

inline int sW = 1280;
inline int sH = 720;
inline std::string WorkingDirectory;