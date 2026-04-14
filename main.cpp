#include "shared.h"
#include "Interface/IPlayer.h"
#include "Interface/ICamera.h"
#include "Interface/IFPSCounter.h"
#include "Interface/IWorld.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#ifdef _DEBUG
    Logger::Initialize("Omnia Debug Console");
#endif

    Image windowLogo = LoadImage("Omnia.png");

    WorkingDirectory = std::filesystem::current_path().string();

    InitWindow(1280, 720, ("Omnia " + OmniaVersion).c_str());
    SetWindowIcon(windowLogo);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    Music leaffall = LoadMusicStream((WorkingDirectory + "\\music\\leaffall.mp3").c_str());
    PlayMusicStream(leaffall);
    SetMusicVolume(leaffall, 0.85f);

    Discord drpc;
    drpc.Initialize("1493574877779853382");
    drpc.Update();

    IPlayer Adelina;
    Adelina.Power();

    ICamera Camera;
    Camera.Power();

    IWorld World;

    IFPSCounter FPSDebug;

    while (!WindowShouldClose()) {
        sW = GetScreenWidth();
        sH = GetScreenHeight();

        if (IsKeyPressed(KEY_F11))
        {
            isFullscreen = !isFullscreen;
            if (isFullscreen)
            {
                int monitor = GetCurrentMonitor();
                int w = GetMonitorWidth(monitor);
                int h = GetMonitorHeight(monitor);
                SetWindowSize(w, h);
                ToggleFullscreen();
            }
            else
            {
                ToggleFullscreen();
                SetWindowSize(1280, 720);
            }
        }

        float deltaTime = GetFrameTime();
        FPSDebug.Update();
        UpdateMusicStream(leaffall);

        Adelina.Fire(deltaTime);
        Camera.Fire(deltaTime, Adelina.pos);
        World.Fire(Adelina.pos);
        World.CleanOldVolumes();

        BeginDrawing();
        ClearBackground(BLACK);

        // Reserved for Camera2D Related Things (Textures, Objects, Etc..)
        Camera.Enter();
        World.Paint();
        Adelina.Paint();

        DrawText("Hello raylib!", 300, 280, 20, RAYWHITE);
        DrawRectangle(300, 300, 50, 50, RED);
        Camera.Destroy();

        // User Interface
        if (IsKeyPressed(KEY_F10)) isDebug = !isDebug;
        if (isDebug) {
            DrawText(TextFormat("FPS: %d\nFrameTime: %.2f ms\n%%1: %.2f ms\n%%0.1: %.2f ms", (int)FPSDebug.GetFPS(), FPSDebug.GetAverage(), FPSDebug.GetP1(), FPSDebug.GetP01()), 10, 10, 16, WHITE);
            DrawText(TextFormat("LocalPlayer X: %.2f Y: %.2f", Adelina.pos.x, Adelina.pos.y), 10, 100, 16, WHITE);
        }

        EndDrawing();
    }

    Adelina.Destroy();
    Logger::Shutdown();
    UnloadMusicStream(leaffall);
    CloseAudioDevice();
    UnloadImage(windowLogo);
    CloseWindow();
    return 0;
}