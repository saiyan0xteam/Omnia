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
inline bool isResDirty = false;
inline bool isDebug = false;
inline bool isMusicMuted = false;
inline int sW = 1280;
inline int sH = 720;
inline std::string WorkingDirectory;
inline std::string OmniaVersion = "V0.4";
inline float musicVolume = 0.85f;
inline float mouseSensitivity = 1.0f;
inline bool vsync = true;
inline bool invertY = false;