#pragma once
#include "CUserInterface.h"
#include "CModLoader.h"
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
  
  int selectedMod = 0;
  float modScrollY = 0.0f;

  bool DrawMenuButton(const char *text, int x, int y, int fontSize, bool active = false) {
    Rectangle bounds = {(float)x, (float)y, (float)MeasureText(text, fontSize), (float)fontSize};
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    Color textColor = active ? PURPLE : (isHovered ? PINK : RAYWHITE);
    DrawText(text, x, y, fontSize, Fade(textColor, alpha));
    return (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && alpha >= 0.9f);
  }

  void DrawSettingControl(const char *label, std::string value, int x, int y, bool *toggle = nullptr, float *val = nullptr, float step = 0.1f, bool disabled = false) {
    DrawText(label, x, y, 20, Fade(disabled ? GRAY : LIGHTGRAY, alpha));
    int valueX = x + 250;
    if (disabled) { DrawText(value.c_str(), valueX, y, 20, Fade(GRAY, alpha)); return; }

    if (toggle) {
      if (DrawMenuButton((*toggle ? "[ ON ]" : "[ OFF ]"), valueX, y, 20)) { *toggle = !(*toggle); isResDirty = true; }
    } else if (val) {
      if (DrawMenuButton("-", valueX, y, 20)) *val -= step;
      DrawText(value.c_str(), valueX + 30, y, 20, Fade(WHITE, alpha));
      if (DrawMenuButton("+", valueX + 100, y, 20)) *val += step;
      if (*val < 0.0f) *val = 0.0f; if (*val > 1.0f) *val = 1.0f;
    } else {
      if (DrawMenuButton(value.c_str(), valueX, y, 20)) {
        resIndex = (resIndex + 1) % resolutions.size();
        sW = resolutions[resIndex].w;
        sH = resolutions[resIndex].h;
        if (!IsWindowFullscreen()) {
          SetWindowSize(sW, sH);
          int m = GetCurrentMonitor();
          SetWindowPosition((GetMonitorWidth(m) - sW) / 2, (GetMonitorHeight(m) - sH) / 2);
        }
        isResDirty = true;
      }
    }
  }

  void Power() override {
    Logger::Success("UI", "CMenu Powered!");
    for (int i = 0; i < (int)resolutions.size(); i++) {
      if (resolutions[i].w == sW && resolutions[i].h == sH) { resIndex = i; break; }
    }
  }

  void Fire(float dt) override {
    if (isVisible) { alpha += fadeSpeed * dt; if (alpha > 1.0f) alpha = 1.0f; }
    else { alpha -= fadeSpeed * dt; if (alpha < 0.0f) alpha = 0.0f; }
  }

  void PaintMenu(bool isOverlay, CModLoader& modLoader) {
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
      if (DrawMenuButton("MODS", 50, screenH - 150, 30)) currentState = MODS;
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
      case GRAPHICS: DrawSettingControl("Resolution", TextFormat("%dx%d", sW, sH), contentX, 150, nullptr, nullptr, 0, isFullscreen); DrawSettingControl("Fullscreen", "", contentX, 190, &isFullscreen); DrawSettingControl("VSync", "", contentX, 230, &vsync); break;
      case KEYBOARD: DrawText("Move Up: W", contentX, 150, 20, Fade(LIGHTGRAY, alpha)); DrawText("Move Down: S", contentX, 190, 20, Fade(LIGHTGRAY, alpha)); break;
      case MOUSE: DrawSettingControl("Sensitivity", TextFormat("%.1f", mouseSensitivity), contentX, 150, nullptr, &mouseSensitivity); DrawSettingControl("Invert Y", "", contentX, 190, &invertY); break;
      case SOUND: DrawSettingControl("Music Muted", "", contentX, 150, &isMusicMuted); DrawSettingControl("Music Volume", TextFormat("%d%%", (int)(musicVolume * 100)), contentX, 190, nullptr, &musicVolume, 0.05f, isMusicMuted); break;
      }
      break;
    }
    case MODS: {
      DrawText("MODS", 50, 50, 40, Fade(WHITE, alpha));
      if (DrawMenuButton("< BACK", 50, screenH - 100, 30)) currentState = MAIN;
      DrawLineEx({300, 140}, {300, (float)screenH - 140}, 2, Fade(GRAY, alpha));

      int listX = 50; int listY = 150; int visibleHeight = screenH - 300; int totalListHeight = (int)modLoader.loadedMods.size() * 35;
      if (totalListHeight > visibleHeight) {
        float wheel = GetMouseWheelMove(); modScrollY += wheel * 25.0f;
        if (modScrollY > 0) modScrollY = 0; if (modScrollY < -(totalListHeight - visibleHeight)) modScrollY = -(totalListHeight - visibleHeight);
      } else modScrollY = 0;

      BeginScissorMode(listX, listY, 240, visibleHeight);
      for (int i = 0; i < (int)modLoader.loadedMods.size(); i++) {
        if (DrawMenuButton(modLoader.loadedMods[i].name.c_str(), listX, listY + (i * 35) + (int)modScrollY, 20, selectedMod == i)) selectedMod = i;
      }
      EndScissorMode();

      if (!modLoader.loadedMods.empty() && selectedMod < (int)modLoader.loadedMods.size()) {
        auto &mod = modLoader.loadedMods[selectedMod];
        int contentX = 330;
        if (mod.icon.id != 0) {
          float iconSize = 64.0f;
          Rectangle source = {0.0f, 0.0f, (float)mod.icon.width, (float)mod.icon.height};
          Rectangle dest = {(float)contentX, 145.0f, iconSize, iconSize};
          DrawTexturePro(mod.icon, source, dest, {0, 0}, 0.0f, Fade(WHITE, alpha));
          int textStartX = contentX + (int)iconSize + 15;
          DrawText(mod.name.c_str(), textStartX, 145, 30, Fade(PURPLE, alpha));
          int nameWidth = MeasureText(mod.name.c_str(), 30);
          int infoX = textStartX + nameWidth + 10;
          DrawText(TextFormat("v%s by %s", mod.version.c_str(), mod.author.c_str()), infoX, 158, 16, Fade(GRAY, alpha));
          DrawText(mod.description.c_str(), textStartX, 185, 16, Fade(RAYWHITE, alpha));
        } else {
          DrawText(mod.name.c_str(), contentX, 145, 30, Fade(PURPLE, alpha));
          int nameWidth = MeasureText(mod.name.c_str(), 30);
          int infoX = contentX + nameWidth + 10;
          DrawText(TextFormat("v%s by %s", mod.version.c_str(), mod.author.c_str()), infoX, 158, 16, Fade(GRAY, alpha));
          DrawText(mod.description.c_str(), contentX, 185, 16, Fade(RAYWHITE, alpha));
        }

        int btnX = screenW - 130; int btnY = 150;
        if (mod.isRunning) { if (DrawMenuButton("STOP", btnX, btnY, 25)) modLoader.StopMod(selectedMod); }
        else { if (DrawMenuButton("START", btnX, btnY, 25)) modLoader.StartMod(selectedMod); }

        // DINAMIK MOD AYARLARI RENDER
        int settingsY = 230;
        DrawLineEx({(float)contentX, (float)settingsY - 10}, {(float)screenW - 50, (float)settingsY - 10}, 1, Fade(PURPLE, alpha * 0.3f));
        DrawText("MOD SETTINGS", contentX, settingsY, 20, Fade(PURPLE, alpha));
        
        int currentSettingY = settingsY + 35;
        if (mod.settings.empty()) {
            DrawText("No settings available for this mod.", contentX, currentSettingY, 16, Fade(DARKGRAY, alpha));
        } else {
            for (auto& s : mod.settings) {
                switch (s->type) {
                    case SettingType::LABEL:
                        DrawText(s->label.c_str(), contentX, currentSettingY, 18, Fade(LIGHTGRAY, alpha));
                        break;
                    case SettingType::CHECKBOX: {
                        bool val = (s->value > 0.5f);
                        if (DrawMenuButton(val ? "[ ON ]" : "[ OFF ]", contentX + 250, currentSettingY, 18)) {
                            s->value = val ? 0.0f : 1.0f;
                            modLoader.TriggerCallback(s);
                        }
                        DrawText(s->label.c_str(), contentX, currentSettingY, 18, Fade(RAYWHITE, alpha));
                        break;
                    }
                    case SettingType::SLIDER_FLOAT: {
                        if (DrawMenuButton("-", contentX + 250, currentSettingY, 18)) { s->value -= 0.1f; modLoader.TriggerCallback(s); }
                        DrawText(TextFormat("%.2f", s->value), contentX + 280, currentSettingY, 18, Fade(WHITE, alpha));
                        if (DrawMenuButton("+", contentX + 350, currentSettingY, 18)) { s->value += 0.1f; modLoader.TriggerCallback(s); }
                        if (s->value < s->min) s->value = s->min; if (s->value > s->max) s->value = s->max;
                        DrawText(s->label.c_str(), contentX, currentSettingY, 18, Fade(RAYWHITE, alpha));
                        break;
                    }
                    case SettingType::SLIDER_INT: {
                        if (DrawMenuButton("-", contentX + 250, currentSettingY, 18)) { s->value -= 1.0f; modLoader.TriggerCallback(s); }
                        DrawText(TextFormat("%d", (int)s->value), contentX + 280, currentSettingY, 18, Fade(WHITE, alpha));
                        if (DrawMenuButton("+", contentX + 350, currentSettingY, 18)) { s->value += 1.0f; modLoader.TriggerCallback(s); }
                        if (s->value < s->min) s->value = s->min; if (s->value > s->max) s->value = s->max;
                        DrawText(s->label.c_str(), contentX, currentSettingY, 18, Fade(RAYWHITE, alpha));
                        break;
                    }
                    case SettingType::BUTTON:
                        if (DrawMenuButton(s->label.c_str(), contentX, currentSettingY, 18)) {
                            s->isTriggered = true;
                            modLoader.TriggerCallback(s);
                        }
                        break;
                }
                currentSettingY += 30;
            }
        }
      } else DrawText("No mods found in /mods folder.", 330, 150, 20, Fade(GRAY, alpha));
      break;
    }
    default: break;
    }
  }
  void Paint() override {}
  void Destroy() override { Logger::Success("UI", "CMenu Destroyed!"); }
};
