#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <raylib.h>
#include <raymath.h>
#include <EasyCPP/Logger.h>
#include <Discord/Discord.h>
#include <Windows.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <unordered_set>

inline bool isFullscreen = false;
inline bool isDebug = false;
inline int sW = 1280;
inline int sH = 720;
inline std::string WorkingDirectory;
inline std::string OmniaVersion = "V0.2";