#include "CModLoader.h"
#include "../Library/JSON/json.hpp"
#include "../Library/miniz/miniz.h"
#include <fstream>

using json = nlohmann::json;

void SetupOmniaPythonModule(py::module_& m, CModLoader* loader) {
    m.doc() = "Omnia Game Engine API";
    py::class_<ModSetting, std::shared_ptr<ModSetting>>(m, "ModSetting")
        .def_property("value", &ModSetting::GetValue, &ModSetting::SetValue)
        .def_readwrite("label", &ModSetting::label)
        .def_readwrite("is_triggered", &ModSetting::isTriggered);
    auto log = m.def_submodule("Logger");
    log.def("Info", [](std::string m, std::string msg) { Logger::Info(m, msg); });
    log.def("Success", [](std::string m, std::string msg) { Logger::Success(m, msg); });
    log.def("Error", [](std::string m, std::string msg) { Logger::Error(m, msg); });
    auto ui = m.def_submodule("UI");
    ui.def("Checkbox", [loader](std::string l, bool d, py::object cb) { auto s = loader->AddCheckbox(l, d); if (s && !cb.is_none()) s->pyCallback = cb; return s; }, py::arg("label"), py::arg("default"), py::arg("callback") = py::none());
    ui.def("SliderFloat", [loader](std::string l, float d, float min, float max, py::object cb) { auto s = loader->AddSliderFloat(l, d, min, max); if (s && !cb.is_none()) s->pyCallback = cb; return s; }, py::arg("label"), py::arg("default"), py::arg("min"), py::arg("max"), py::arg("callback") = py::none());
    ui.def("SliderInt", [loader](std::string l, int d, int min, int max, py::object cb) { auto s = loader->AddSliderInt(l, d, min, max); if (s && !cb.is_none()) s->pyCallback = cb; return s; }, py::arg("label"), py::arg("default"), py::arg("min"), py::arg("max"), py::arg("callback") = py::none());
    ui.def("Label", [loader](std::string t) { return loader->AddLabel(t); });
    ui.def("Button", [loader](std::string t, py::object cb) { auto s = loader->AddButton(t); if (s && !cb.is_none()) s->pyCallback = cb; return s; }, py::arg("label"), py::arg("callback") = py::none());
    m.def("GetSetting", [loader](std::string mod, std::string label) { return loader->GetSettingValue(mod, label); });
}

void CModLoader::Init() {
    Logger::Info("ModLoader", "Initializing Modding Engines...");
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string);
    ExportLuaAPI();
    try {
        static py::scoped_interpreter guard{};
        py::module_ omnia = py::module_::create_extension_module("omnia", "Omnia Game Engine API", new py::module_::module_def);
        SetupOmniaPythonModule(omnia, this);
        py::module_::import("sys").attr("modules")["omnia"] = omnia;
        Logger::Success("ModLoader", "Python Engine Ready!");
    } catch (const std::exception& e) { Logger::Error("ModLoader", "Python Init Failed: " + std::string(e.what())); }
}

void CModLoader::LoadMods() {
    std::string modsPath = WorkingDirectory + "/mods";
    std::string cachePath = modsPath + "/.cache";
    if (!fs::exists(modsPath)) fs::create_directory(modsPath);
    if (fs::exists(cachePath)) fs::remove_all(cachePath);
    fs::create_directory(cachePath);
    for (auto& mod : loadedMods) if (mod.icon.id != 0) UnloadTexture(mod.icon);
    loadedMods.clear();
    for (const auto& entry : fs::directory_iterator(modsPath)) {
        std::string path = entry.path().string();
        std::string ext = entry.path().extension().string();
        if (ext == ".ocx") {
            std::string modName = entry.path().stem().string();
            std::string targetDir = cachePath + "/" + modName;
            fs::create_directory(targetDir);
            
            // Direct miniz Extraction
            mz_zip_archive zip_archive;
            memset(&zip_archive, 0, sizeof(zip_archive));
            if (mz_zip_reader_init_file(&zip_archive, path.c_str(), 0)) {
                int num_files = mz_zip_reader_get_num_files(&zip_archive);
                bool success = true;
                for (int i = 0; i < num_files; i++) {
                    mz_zip_archive_file_stat file_stat;
                    if (mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
                        std::string destFile = targetDir + "/" + file_stat.m_filename;
                        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
                            fs::create_directories(destFile);
                        } else {
                            fs::path p(destFile);
                            if (p.has_parent_path()) fs::create_directories(p.parent_path());
                            if (!mz_zip_reader_extract_to_file(&zip_archive, i, destFile.c_str(), 0)) {
                                Logger::Error("ModLoader", "Failed to extract file: " + std::string(file_stat.m_filename));
                                success = false;
                                break;
                            }
                        }
                    }
                }
                mz_zip_reader_end(&zip_archive);
                if (success) {
                    LoadMod(targetDir);
                    Logger::Success("ModLoader", "Loaded archive: " + modName + ".ocx");
                }
            } else {
                Logger::Error("ModLoader", "Failed to open archive: " + modName + ".ocx");
            }
        } else if (fs::is_directory(entry.path()) && entry.path().filename().string()[0] != '.') {
            LoadMod(path);
        }
    }
}

void CModLoader::FireMods() {
    for (auto& mod : loadedMods) {
        if (!mod.isRunning) continue;
        if (mod.luaLoop.valid()) {
            auto res = mod.luaLoop();
            if (!res.valid()) { sol::error err = res; Logger::Error(mod.name, "Lua Loop Fail: " + std::string(err.what())); mod.isRunning = false; }
        }
        if (mod.pyLoop && !mod.pyLoop.is_none()) {
            try { mod.pyLoop(); } catch (py::error_already_set& e) { Logger::Error(mod.name, "Python Loop Fail: " + std::string(e.what())); mod.isRunning = false; }
        }
    }
}

void CModLoader::StartMod(int index) {
    if (index < 0 || index >= (int)loadedMods.size()) return;
    ModInfo& mod = loadedMods[index];
    if (mod.isRunning) return;
    mod.isRunning = true;
    if (mod.luaBoot.valid()) {
        auto res = mod.luaBoot();
        if (!res.valid()) { sol::error err = res; Logger::Error(mod.name, "Lua Boot Fail: " + std::string(err.what())); mod.isRunning = false; }
    }
    if (mod.pyBoot && !mod.pyBoot.is_none()) {
        try { mod.pyBoot(); } catch (py::error_already_set& e) { Logger::Error(mod.name, "Python Boot Fail: " + std::string(e.what())); mod.isRunning = false; }
    }
    if (mod.isRunning) Logger::Success("ModLoader", mod.name + " is now active!");
}

void CModLoader::StopMod(int index) {
    if (index < 0 || index >= (int)loadedMods.size()) return;
    ModInfo& mod = loadedMods[index];
    if (!mod.isRunning) return;
    if (mod.luaKill.valid()) mod.luaKill();
    if (mod.pyKill && !mod.pyKill.is_none()) try { mod.pyKill(); } catch (...) {}
    mod.isRunning = false;
    Logger::Info("ModLoader", mod.name + " stopped.");
}

float CModLoader::GetSettingValue(std::string modName, std::string label) {
    for (auto& m : loadedMods) {
        if (m.name == modName) {
            for (auto& s : m.settings) if (s->label == label) return s->value;
        }
    }
    return 0.0f;
}

void CModLoader::TriggerCallback(std::shared_ptr<ModSetting> s) {
    if (!s) return;
    if (s->luaCallback.valid()) {
        auto res = s->luaCallback(s->value);
        if (!res.valid()) { sol::error err = res; Logger::Error("ModLoader", "Lua Callback Fail: " + std::string(err.what())); }
    }
    if (s->pyCallback && !s->pyCallback.is_none()) {
        try { s->pyCallback(s->value); } catch (py::error_already_set& e) { Logger::Error("ModLoader", "Python Callback Fail: " + std::string(e.what())); }
    }
}

std::shared_ptr<ModSetting> CModLoader::AddCheckbox(std::string label, bool def) {
    if (!currentLoadingMod) return nullptr;
    auto s = std::make_shared<ModSetting>();
    s->type = SettingType::CHECKBOX; s->label = label; s->value = def ? 1.0f : 0.0f;
    currentLoadingMod->settings.push_back(s);
    return s;
}

std::shared_ptr<ModSetting> CModLoader::AddSliderFloat(std::string label, float def, float min, float max) {
    if (!currentLoadingMod) return nullptr;
    auto s = std::make_shared<ModSetting>();
    s->type = SettingType::SLIDER_FLOAT; s->label = label; s->value = def; s->min = min; s->max = max;
    currentLoadingMod->settings.push_back(s);
    return s;
}

std::shared_ptr<ModSetting> CModLoader::AddSliderInt(std::string label, int def, int min, int max) {
    if (!currentLoadingMod) return nullptr;
    auto s = std::make_shared<ModSetting>();
    s->type = SettingType::SLIDER_INT; s->label = label; s->value = (float)def; s->min = (float)min; s->max = (float)max;
    currentLoadingMod->settings.push_back(s);
    return s;
}

std::shared_ptr<ModSetting> CModLoader::AddLabel(std::string text) {
    if (!currentLoadingMod) return nullptr;
    auto s = std::make_shared<ModSetting>();
    s->type = SettingType::LABEL; s->label = text;
    currentLoadingMod->settings.push_back(s);
    return s;
}

std::shared_ptr<ModSetting> CModLoader::AddButton(std::string text) {
    if (!currentLoadingMod) return nullptr;
    auto s = std::make_shared<ModSetting>();
    s->type = SettingType::BUTTON; s->label = text;
    currentLoadingMod->settings.push_back(s);
    return s;
}

void CModLoader::ExportLuaAPI() {
    lua.new_usertype<ModSetting>("ModSetting", "value", sol::property(&ModSetting::GetValue, &ModSetting::SetValue), "label", &ModSetting::label, "is_triggered", &ModSetting::isTriggered);
    auto log = lua.create_table("Logger");
    log["Info"] = [](std::string m, std::string msg) { Logger::Info(m, msg); };
    log["Success"] = [](std::string m, std::string msg) { Logger::Success(m, msg); };
    log["Error"] = [](std::string m, std::string msg) { Logger::Error(m, msg); };
    auto ui = lua.create_table("UI");
    ui["Checkbox"] = [this](std::string l, bool d, sol::optional<sol::function> cb) { auto s = AddCheckbox(l, d); if (s && cb) s->luaCallback = *cb; return s; };
    ui["SliderFloat"] = [this](std::string l, float d, float min, float max, sol::optional<sol::function> cb) { auto s = AddSliderFloat(l, d, min, max); if (s && cb) s->luaCallback = *cb; return s; };
    ui["SliderInt"] = [this](std::string l, int d, int min, int max, sol::optional<sol::function> cb) { auto s = AddSliderInt(l, d, min, max); if (s && cb) s->luaCallback = *cb; return s; };
    ui["Label"] = [this](std::string t) { return AddLabel(t); };
    ui["Button"] = [this](std::string t, sol::optional<sol::function> cb) { auto s = AddButton(t); if (s && cb) s->luaCallback = *cb; return s; };
    lua["GetSetting"] = [this](std::string m, std::string l) { return GetSettingValue(m, l); };
}

void CModLoader::LoadMod(const std::string& path) {
    std::string infoPath = path + "/modinfo.json";
    ModInfo info; info.folderPath = path; info.name = fs::path(path).filename().string(); info.icon = { 0 };
    if (fs::exists(infoPath)) {
        try {
            std::ifstream f(infoPath); json data = json::parse(f);
            info.name = data.value("name", info.name);
            info.description = data.value("description", "No description.");
            info.version = data.value("version", "0.0.1");
            info.author = data.value("author", "Unknown");
            std::string iconPath = path + "/" + data.value("icon", "logo.png");
            if (fs::exists(iconPath)) info.icon = LoadTexture(iconPath.c_str());
        } catch (...) {}
    }
    currentLoadingMod = &info;
    std::string luaMain = path + "/main.lua";
    if (fs::exists(luaMain)) {
        try {
            lua.script_file(luaMain);
            info.luaBoot = lua["Boot"]; info.luaLoop = lua["Loop"]; info.luaKill = lua["Kill"]; info.luaSettings = lua["Settings"];
            if (info.luaSettings.valid()) info.luaSettings();
        } catch (const sol::error& e) { Logger::Error(info.name, "Lua Load Error: " + std::string(e.what())); }
    }
    std::string pyMain = path + "/main.py";
    if (fs::exists(pyMain)) {
        try {
            py::module_ sys = py::module_::import("sys");
            sys.attr("path").attr("insert")(0, path); 
            py::dict mod_globals = py::dict(py::globals());
            mod_globals["omnia"] = py::module_::import("omnia");
            py::eval_file(pyMain, mod_globals, mod_globals);
            if (mod_globals.contains("Boot")) info.pyBoot = mod_globals["Boot"];
            if (mod_globals.contains("Loop")) info.pyLoop = mod_globals["Loop"];
            if (mod_globals.contains("Kill")) info.pyKill = mod_globals["Kill"];
            if (mod_globals.contains("Settings")) {
                info.pySettings = mod_globals["Settings"];
                info.pySettings();
            }
            Logger::Success("ModLoader", "Python scripts for " + info.name + " loaded.");
        } catch (const py::error_already_set &e) { Logger::Error(info.name, "Python Load Error: " + std::string(e.what())); }
    }
    loadedMods.push_back(info);
    currentLoadingMod = nullptr;
}

void CModLoader::Destroy() {
    for (int i = 0; i < (int)loadedMods.size(); i++) if (loadedMods[i].isRunning) StopMod(i);
    for (auto& mod : loadedMods) if (mod.icon.id != 0) UnloadTexture(mod.icon);
    loadedMods.clear();
}
