#pragma once
#include "../shared.h"
#include <sol/sol.hpp>
#include <pybind11/embed.h>
#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <raylib.h>

namespace fs = std::filesystem;
namespace py = pybind11;

enum class SettingType { CHECKBOX, SLIDER_INT, SLIDER_FLOAT, TEXTBOX, KEYBIND, LABEL, BUTTON };

struct ModSetting {
    SettingType type;
    std::string label;
    float value = 0.0f;
    std::string textValue;
    int key = 0;
    float min = 0.0f;
    float max = 1.0f;
    bool isTriggered = false;

    // Callbacks
    sol::function luaCallback;
    py::object pyCallback;

    float GetValue() const { return value; }
    void SetValue(float v) { value = v; }
};

struct ModInfo {
    std::string name;
    std::string description;
    std::string version;
    std::string author;
    std::string folderPath;
    Texture2D icon;
    
    bool isRunning = false;
    std::vector<std::shared_ptr<ModSetting>> settings;
    
    sol::function luaBoot, luaLoop, luaKill, luaSettings;
    py::object pyBoot, pyLoop, pyKill, pySettings, pyModule;
};

class CModLoader {
public:
    sol::state lua;
    std::vector<ModInfo> loadedMods;
    ModInfo* currentLoadingMod = nullptr;

    void Init();
    void LoadMods();
    void FireMods();
    void StartMod(int index);
    void StopMod(int index);
    void Destroy();

    std::shared_ptr<ModSetting> AddCheckbox(std::string label, bool def);
    std::shared_ptr<ModSetting> AddSliderFloat(std::string label, float def, float min, float max);
    std::shared_ptr<ModSetting> AddSliderInt(std::string label, int def, int min, int max);
    std::shared_ptr<ModSetting> AddLabel(std::string text);
    std::shared_ptr<ModSetting> AddButton(std::string text);
    float GetSettingValue(std::string modName, std::string label);

    // Callback Tetikleyici (CMenu tarafından çağrılır)
    void TriggerCallback(std::shared_ptr<ModSetting> setting);

private:
    void ExportLuaAPI();
    void LoadMod(const std::string& path);
};
