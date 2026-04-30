#include "CModLoader.h"
#include <pybind11/embed.h>

namespace py = pybind11;

// Manuel modül tanımlama fonksiyonu (Makro hatasından kaçınmak için)
void SetupOmniaPythonModule(py::module_& m) {
    m.doc() = "Omnia Game Engine API";
    
    auto log = m.def_submodule("Logger");
    log.def("Info", [](std::string m, std::string msg) { Logger::Info(m, msg); });
    log.def("Success", [](std::string m, std::string msg) { Logger::Success(m, msg); });
    log.def("Error", [](std::string m, std::string msg) { Logger::Error(m, msg); });
}

void CModLoader::Init() {
    Logger::Info("ModLoader", "Initializing Modding Engines...");
    
    // Initialize Lua
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string);
    ExportLuaAPI();

    // Initialize Python
    try {
        static py::scoped_interpreter guard{};
        
        // Modülü manuel olarak ekle
        auto m = py::module_::import("__main__"); 
        // Alternatif olarak 'omnia' isminde yeni bir modül oluşturup sys.modules'a ekleyelim
        py::module_ omnia = py::module_::create_extension_module("omnia", "Omnia Game Engine API", new py::module_::module_def);
        SetupOmniaPythonModule(omnia);
        py::module_::import("sys").attr("modules")["omnia"] = omnia;

        Logger::Success("ModLoader", "Python Engine Ready!");
    } catch (const std::exception& e) {
        Logger::Error("ModLoader", "Python Init Failed: " + std::string(e.what()));
    }

    Logger::Success("ModLoader", "Modding Engines Ready!");
}

void CModLoader::LoadMods() {
    std::string modsPath = WorkingDirectory + "/mods";
    if (!fs::exists(modsPath)) {
        fs::create_directory(modsPath);
    }

    for (const auto& entry : fs::directory_iterator(modsPath)) {
        if (entry.path().extension() == ".ocx" || fs::is_directory(entry.path())) {
            LoadMod(entry.path().string());
        }
    }
}

void CModLoader::ExportLuaAPI() {
    auto log = lua.create_table("Logger");
    log["Info"] = [](std::string m, std::string msg) { Logger::Info(m, msg); };
    log["Success"] = [](std::string m, std::string msg) { Logger::Success(m, msg); };
    log["Error"] = [](std::string m, std::string msg) { Logger::Error(m, msg); };

    lua["RED"] = RED;
    lua["GREEN"] = GREEN;
    lua["BLUE"] = BLUE;
    lua["WHITE"] = WHITE;
    lua["BLACK"] = BLACK;
}

void CModLoader::LoadMod(const std::string& path) {
    std::string luaMain = path + "/main.lua";
    std::string pyMain = path + "/main.py";

    if (fs::exists(luaMain)) {
        try {
            lua.script_file(luaMain);
            Logger::Success("ModLoader", "Lua mod loaded: " + path);
        } catch (const sol::error& e) {
            Logger::Error("ModLoader", "Lua Error: " + std::string(e.what()));
        }
    }
    
    if (fs::exists(pyMain)) {
        try {
            py::module_ sys = py::module_::import("sys");
            sys.attr("path").attr("append")(path);
            py::module_ mod = py::module_::import("main");
            Logger::Success("ModLoader", "Python mod loaded: " + path);
        } catch (const py::error_already_set &e) {
            Logger::Error("ModLoader", "Python Error: " + std::string(e.what()));
        }
    }
}
