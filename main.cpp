#include "shared.h"
#include "Interface/IPlayer.h"
#include "Interface/ICamera.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#ifdef _DEBUG
    Logger::Initialize("Omnia Debug Console");
#endif

    WorkingDirectory = std::filesystem::current_path().string();

    InitWindow(1280, 720, "Omnia V0.1");
    SetTargetFPS(60);

    sW = GetScreenWidth();
    sH = GetScreenHeight();

    IPlayer Adelina;
    Adelina.Power();

    ICamera Camera;
    Camera.Power();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Camera.Fire(deltaTime, Adelina.pos);
        BeginDrawing();
        ClearBackground(BLACK);
        Camera.Enter();
        Adelina.Fire(deltaTime);
        DrawText("Hello raylib!", 300, 280, 20, RAYWHITE);
        DrawRectangle(300, 300, 50, 50, RED);
        Camera.Destroy();
        EndDrawing();
    }

    Adelina.Destroy();
    Logger::Shutdown();
    CloseWindow();
    return 0;
}