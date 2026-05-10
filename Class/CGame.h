#pragma once
#include "../shared.h"
#include "CDebug.h"
#include "CMenu.h"
#include "CModLoader.h"
#include "CPlayer.h"
#include "CWorld.h"
#include "CCamera.h"
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
  bool isPaused = false; // Pause durumunu takip et

  CModLoader ModLoader;
  CDebug Debug;
  CMenu Menu;
  CPlayer Player;
  CWorld World;
  CCamera Camera;

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
      sW = GetMonitorWidth(m);
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
    ModLoader.LoadMods(); // Modları tara ve JSON verilerini oku
    Player.Power();
    Camera.Power();
    Debug.Power();
    Menu.Power();
  }

  void Update() {
    float realDeltaTime = GetFrameTime();
    // Pause durumunda oyunu durdur (deltaTime = 0)
    float dt = (gameState == MENU) ? 0.0f : realDeltaTime;

    Debug.Update();
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
        sW = mW;
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

    if (vsync) SetTargetFPS(60); else SetTargetFPS(0);

    if (gameState == MENU) {
      Menu.Fire(realDeltaTime);
      
      // Pause menüsündeyken ESC ile kapatma/geri dönme
      if (isPaused && IsKeyPressed(KEY_ESCAPE)) {
          if (Menu.currentState != CMenu::MAIN) {
              Menu.currentState = CMenu::MAIN;
          } else {
              Menu.isVisible = false; // Oyuna dön
          }
      }

      // Ana Menüye Dönüş
      if (Menu.shouldGoToTitle) {
        gameState = MENU;
        isPaused = false; // Artık ana menüdeyiz, pause değil
        Menu.shouldGoToTitle = false;
        Menu.isVisible = true;
        Menu.currentState = CMenu::MAIN;
        Player.pos = {0, 0};
        Logger::Info("Game", "Returned to Main Menu");
      }

      // Menü kapandığında oyuna geç
      if (!Menu.isVisible && Menu.alpha <= 0.0f) {
        gameState = INGAME;
        isPaused = false;
      }
    } else {
      Player.Fire(dt);
      World.Fire(Player.pos);
      Camera.Fire(realDeltaTime, Player.pos);

      if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = MENU;
        isPaused = true; // Oyunu duraklat
        Menu.isVisible = true;
        Menu.currentState = CMenu::MAIN;
      }
    }

    // Aktif Modların Loop'larını Çalıştır
    ModLoader.FireMods();
  }

  void Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    // Oyun dünyasını çiz (Eğer in-game isek veya pause menüsü açıksa)
    if (gameState == INGAME || isPaused) {
      Camera.Enter();
        World.Paint();
        Player.Paint();
      EndMode2D();
    }

    if (gameState == MENU) {
      // isPaused true ise saydam siyah arka planla çizilecek
      Menu.PaintMenu(isPaused, ModLoader);
    }

    if (gameState == INGAME) {
        Debug.Paint(Player, World);
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
    Player.Destroy();
    Camera.Destroy();
    Debug.Destroy();
    ModLoader.Destroy(); // İkonları ve motorları temizle
    Menu.Destroy();
    CloseWindow();
  }
};
