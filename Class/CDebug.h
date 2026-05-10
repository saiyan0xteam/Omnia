#pragma once
#include "../shared.h"
#include "CPlayer.h"
#include "CWorld.h"
#include <vector>
#include <algorithm>

class CDebug {
public:
    std::vector<float> frameTimes;
    const size_t maxSamples = 300;
    bool isVisible = false;

    void Power() {
        Logger::Success("Interfaces", "CDebug System Powered!");
    }

    void Update() {
        float dt = GetFrameTime() * 1000.0f;
        frameTimes.push_back(dt);
        if (frameTimes.size() > maxSamples)
            frameTimes.erase(frameTimes.begin());

        if (IsKeyPressed(KEY_F10)) isVisible = !isVisible;
    }

    float GetFPS() {
        float avg = GetAverage();
        return (avg > 0) ? 1000.0f / avg : 0.0f;
    }

    float GetAverage() {
        float sum = 0;
        for (float t : frameTimes) sum += t;
        return frameTimes.empty() ? 0 : sum / frameTimes.size();
    }

    void Paint(const CPlayer& player, const CWorld& world) {
        if (!isVisible) return;

        // Arka plan paneli (Hafif siyah opak)
        DrawRectangle(10, 10, 250, 150, Fade(BLACK, 0.7f));
        DrawRectangleLines(10, 10, 250, 150, Fade(PURPLE, 0.5f));

        int x = 20;
        int y = 20;
        int fontSize = 15;
        Color textColor = RAYWHITE;

        // Performans Bilgileri
        DrawText(TextFormat("FPS: %.1f", GetFPS()), x, y, fontSize, textColor);
        DrawText(TextFormat("Frame Time: %.2f ms", GetAverage()), x, y += 20, fontSize, textColor);

        // Oyuncu Bilgileri
        DrawText(TextFormat("Player Pos: [%.1f, %.1f]", player.pos.x, player.pos.y), x, y += 30, fontSize, textColor);
        
        // Dünya Bilgileri
        int loadedChunks = (int)world.visibleVolumes.size();
        DrawText(TextFormat("Visible Chunks: %d", loadedChunks), x, y += 20, fontSize, textColor);
        
        // Versiyon Bilgisi
        DrawText(TextFormat("Omnia Version: %s", OmniaVersion.c_str()), x, y += 30, fontSize, GRAY);
    }

    void Destroy() {
        Logger::Success("Interfaces", "CDebug System Destroyed!");
    }
};
