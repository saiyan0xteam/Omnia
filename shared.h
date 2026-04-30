#pragma once
#include <Discord/Discord.h>
#include <EasyCPP/Logger.h>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

inline bool isFullscreen = false;
inline bool isDebug = false;
inline int sW = 1280;
inline int sH = 720;
inline std::string WorkingDirectory;
inline std::string OmniaVersion = "V0.3";