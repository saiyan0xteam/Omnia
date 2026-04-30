#pragma once
#include "../shared.h"
#include "CFPSCounter.h"
#include "CMenu.h"
#include "CModLoader.h"
#include <JSON/json.hpp>
#include <filesystem>
#include <fstream>
#include <raylib.h>
#include <string>

using json = nlohmann::json;

class CGame {
public:
  enum GameState { MENU, INGAME };
  GameState gameState = MENU;

  CModLoader ModLoader;
  CFPSCounter FPSDebug;
  CMenu Menu;

  Image windowLogo;
  Music leaffall;

  void LoadSettings() {
    std::string path = WorkingDirectory + "/settings.json";
    if (std::filesystem::exists(path)) {
      try {
        std::ifstream f(path);
        json data = json::parse(f);

        isFullscreen = data.value("isFullscreen", false);
        vsync = data.value("vsync", true);
        sW = data.value("sW", 1280);
        sH = data.value("sH", 720);
        musicVolume = data.value("musicVolume", 0.85f);
        isMusicMuted = data.value("isMusicMuted", false);
        mouseSensitivity = data.value("mouseSensitivity", 1.0f);
        invertY = data.value("invertY", false);

        Logger::Success("Settings", "Settings loaded from settings.json");
      } catch (std::exception &e) {
        Logger::Error("Settings", "Failed to parse settings.json: " + std::string(e.what()));
      }
    } else {
      SaveSettings();
    }
  }

  void SaveSettings() {
    std::string path = WorkingDirectory + "/settings.json";
    json data;
    data["isFullscreen"] = isFullscreen;
    data["vsync"] = vsync;
    data["sW"] = sW;
    data["sH"] = sH;
    data["musicVolume"] = musicVolume;
    data["isMusicMuted"] = isMusicMuted;
    data["mouseSensitivity"] = mouseSensitivity;
    data["invertY"] = invertY;

    std::ofstream f(path);
    f << data.dump(4);
    Logger::Success("Settings", "Settings saved to settings.json");
  }

  void Init() {
    windowLogo = LoadImage("Omnia.png");
    WorkingDirectory = std::filesystem::current_path().string();

    LoadSettings();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(sW, sH, ("Omnia " + OmniaVersion).c_str());
    SetWindowIcon(windowLogo);

    if (isFullscreen) {
      int m = GetCurrentMonitor();
      sW = GetMonitorWidth(m); // Açılışta sW/sH'ı güncelle
      sH = GetMonitorHeight(m);
      SetWindowSize(sW, sH);
      ToggleFullscreen();
    }

    SetTargetFPS(vsync ? 60 : 0);
    SetExitKey(KEY_NULL);

    InitAudioDevice();
    leaffall = LoadMusicStream("Assets/Music/leaffall.mp3");
    PlayMusicStream(leaffall);

    ModLoader.Init();
    Menu.Power();
  }

  void Update() {
    float realDeltaTime = GetFrameTime();

    FPSDebug.Update();
    UpdateMusicStream(leaffall);
    SetMusicVolume(leaffall, isMusicMuted ? 0.0f : musicVolume);

    if (IsKeyPressed(KEY_F11)) {
      isFullscreen = !isFullscreen;
      isResDirty = true;
    }

    if (isResDirty || (IsWindowFullscreen() != isFullscreen)) {
      int monitor = GetCurrentMonitor();
      int mW = GetMonitorWidth(monitor);
      int mH = GetMonitorHeight(monitor);

      if (isFullscreen) {
        if (!IsWindowFullscreen())
          ToggleFullscreen();
        sW = mW; // sW ve sH'ı monitör boyutuna eşitle
        sH = mH;
        SetWindowSize(sW, sH);
      } else {
        if (IsWindowFullscreen())
          ToggleFullscreen();
        SetWindowSize(sW, sH);
        SetWindowPosition((mW - sW) / 2, (mH - sH) / 2);
      }

      isResDirty = false;
      isFullscreen = IsWindowFullscreen();
      SaveSettings();
    }

    if (vsync)
      SetTargetFPS(60);
    else
      SetTargetFPS(0);

    if (gameState == MENU) {
      Menu.Fire(realDeltaTime);
      if (Menu.shouldGoToTitle) {
        gameState = MENU;
        Menu.shouldGoToTitle = false;
        Menu.isVisible = true;
        Menu.currentState = CMenu::MAIN;
      }
      if (!Menu.isVisible && Menu.alpha <= 0.0f) {
        gameState = INGAME;
      }
    } else {
      if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = MENU;
        Menu.isVisible = true;
      }
    }
  }

  void Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    if (gameState == INGAME) {
      DrawText("GAME IS RUNNING...", 100, 100, 20, RAYWHITE);
    }

    if (gameState == MENU) {
      Menu.PaintMenu(gameState == INGAME);
    }

    EndDrawing();
  }

  void Run() {
    while (!WindowShouldClose()) {
      Update();
      Draw();
    }
    Shutdown();
  }

  void Shutdown() {
    SaveSettings();
    UnloadMusicStream(leaffall);
    CloseAudioDevice();
    Menu.Destroy();
    CloseWindow();
  }
};
