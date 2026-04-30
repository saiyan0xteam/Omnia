#pragma once
#include "CUserInterface.h"
#include <vector>

struct Resolution { int w; int h; };

class CMenu : public CUserInterface {
public:
  enum MenuState { MAIN, SETTINGS, MODS, NONE };
  enum SettingsTab { GRAPHICS, KEYBOARD, MOUSE, SOUND };

  MenuState currentState = MAIN;
  SettingsTab currentTab = GRAPHICS;
  bool isVisible = true;
  bool shouldGoToTitle = false;

  float alpha = 1.0f;
  float fadeSpeed = 4.0f;

  std::vector<Resolution> resolutions = { {1280, 720}, {1600, 900}, {1920, 1080} };
  int resIndex = 0;

  bool DrawMenuButton(const char *text, int x, int y, int fontSize, bool active = false) {
    Rectangle bounds = {(float)x, (float)y, (float)MeasureText(text, fontSize), (float)fontSize};
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    Color textColor = active ? PURPLE : (isHovered ? PINK : RAYWHITE);
    DrawText(text, x, y, fontSize, Fade(textColor, alpha));
    return (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && alpha >= 0.9f);
  }

  void DrawSettingControl(const char* label, std::string value, int x, int y, bool* toggle = nullptr, float* val = nullptr, float step = 0.1f, bool disabled = false) {
    DrawText(label, x, y, 20, Fade(disabled ? GRAY : LIGHTGRAY, alpha));
    int valueX = x + 250;

    if (disabled) {
        DrawText(value.c_str(), valueX, y, 20, Fade(GRAY, alpha));
        return;
    }

    if (toggle) {
        if (DrawMenuButton((*toggle ? "[ ON ]" : "[ OFF ]"), valueX, y, 20)) {
            *toggle = !(*toggle);
            isResDirty = true;
        }
    } else if (val) {
        if (DrawMenuButton("-", valueX, y, 20)) *val -= step;
        DrawText(value.c_str(), valueX + 30, y, 20, Fade(WHITE, alpha));
        if (DrawMenuButton("+", valueX + 100, y, 20)) *val += step;
        if (*val < 0.0f) *val = 0.0f; if (*val > 1.0f) *val = 1.0f;
    } else {
        // Çözünürlük Değiştirme (DOĞRUDAN MÜDAHALE)
        if (DrawMenuButton(value.c_str(), valueX, y, 20)) {
            resIndex = (resIndex + 1) % resolutions.size();
            sW = resolutions[resIndex].w;
            sH = resolutions[resIndex].h;
            
            // Eğer pencereli moddaysak hemen boyutlandır ve ortala
            if (!IsWindowFullscreen()) {
                SetWindowSize(sW, sH);
                int m = GetCurrentMonitor();
                SetWindowPosition((GetMonitorWidth(m) - sW) / 2, (GetMonitorHeight(m) - sH) / 2);
            }
            
            isResDirty = true;
            Logger::Info("Menu", "Forcing window resize: " + std::to_string(sW) + "x" + std::to_string(sH));
        }
    }
  }

  void Power() override { 
      Logger::Success("UI", "CMenu Powered!");
      for(int i = 0; i < (int)resolutions.size(); i++) {
          if(resolutions[i].w == sW && resolutions[i].h == sH) {
              resIndex = i;
              break;
          }
      }
  }

  void Fire(float dt) override {
    if (isVisible) { alpha += fadeSpeed * dt; if (alpha > 1.0f) alpha = 1.0f; }
    else { alpha -= fadeSpeed * dt; if (alpha < 0.0f) alpha = 0.0f; }
  }

  void PaintMenu(bool isOverlay) {
    if (alpha <= 0.0f) return;
    int m = GetCurrentMonitor();
    int screenW = IsWindowFullscreen() ? GetMonitorWidth(m) : GetScreenWidth();
    int screenH = IsWindowFullscreen() ? GetMonitorHeight(m) : GetScreenHeight();

    if (isOverlay) DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, alpha * 0.7f));
    else DrawRectangle(0, 0, screenW, screenH, BLACK);

    switch (currentState) {
    case MAIN:
      DrawText(isOverlay ? "PAUSED" : "OMNIA", 50, screenH / 2 - 100, 80, Fade(WHITE, alpha));
      if (!isOverlay) DrawText(OmniaVersion.c_str(), 320, screenH / 2 - 40, 20, Fade(GRAY, alpha));
      if (DrawMenuButton(isOverlay ? "RESUME" : "PLAY", 50, screenH - 250, 30)) isVisible = false;
      if (DrawMenuButton("SETTINGS", 50, screenH - 200, 30)) currentState = SETTINGS;
      if (DrawMenuButton("MOD MANAGEMENT", 50, screenH - 150, 30)) currentState = MODS;
      if (isOverlay) { if (DrawMenuButton("BACK TO MAIN MENU", 50, screenH - 100, 30)) shouldGoToTitle = true; }
      else { if (DrawMenuButton("QUIT", 50, screenH - 100, 30)) CloseWindow(); }
      break;

    case SETTINGS: {
      DrawText("SETTINGS", 50, 50, 40, Fade(WHITE, alpha));
      if (DrawMenuButton("GRAPHICS", 50, 150, 25, currentTab == GRAPHICS)) currentTab = GRAPHICS;
      if (DrawMenuButton("KEYBOARD", 50, 200, 25, currentTab == KEYBOARD)) currentTab = KEYBOARD;
      if (DrawMenuButton("MOUSE", 50, 250, 25, currentTab == MOUSE)) currentTab = MOUSE;
      if (DrawMenuButton("SOUND", 50, 300, 25, currentTab == SOUND)) currentTab = SOUND;
      if (DrawMenuButton("< BACK", 50, screenH - 100, 30)) currentState = MAIN;

      DrawLineEx({250, 140}, {250, (float)screenH - 140}, 2, Fade(GRAY, alpha));
      int contentX = 300;
      switch (currentTab) {
      case GRAPHICS:
        DrawSettingControl("Resolution", TextFormat("%dx%d", sW, sH), contentX, 150, nullptr, nullptr, 0, isFullscreen);
        DrawSettingControl("Fullscreen", "", contentX, 190, &isFullscreen);
        DrawSettingControl("VSync", "", contentX, 230, &vsync);
        break;
      case KEYBOARD:
        DrawText("Move Up: W", contentX, 150, 20, Fade(LIGHTGRAY, alpha));
        DrawText("Move Down: S", contentX, 190, 20, Fade(LIGHTGRAY, alpha));
        break;
      case MOUSE:
        DrawSettingControl("Sensitivity", TextFormat("%.1f", mouseSensitivity), contentX, 150, nullptr, &mouseSensitivity);
        DrawSettingControl("Invert Y", "", contentX, 190, &invertY);
        break;
      case SOUND:
        DrawSettingControl("Music Muted", "", contentX, 150, &isMusicMuted);
        DrawSettingControl("Music Volume", TextFormat("%d%%", (int)(musicVolume * 100)), contentX, 190, nullptr, &musicVolume, 0.05f, isMusicMuted);
        break;
      }
      break;
    }
    case MODS:
      DrawText("MOD MANAGEMENT", 50, 100, 40, Fade(WHITE, alpha));
      if (DrawMenuButton("< BACK", 50, screenH - 100, 30)) currentState = MAIN;
      break;
    default: break;
    }
  }
  void Paint() override {}
  void Destroy() override { Logger::Success("UI", "CMenu Destroyed!"); }
};
