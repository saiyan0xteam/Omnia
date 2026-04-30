#pragma once
#include "../shared.h"
#include <sol/sol.hpp>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class CModLoader {
public:
    sol::state lua;

    void Init();
    void LoadMods();

private:
    void ExportLuaAPI();
    void LoadMod(const std::string& path);
};
