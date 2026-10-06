#include "core.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // Initialize graphics, SDL3 window, and OpenGL context
    if (!Renderer_Init(1280, 720, "2D MMO Engine - Core Architecture"))
    {
        std::cerr << "[Fatal Error] Failed to initialize rendering context." << std::endl;
        return -1;
    }

    std::cout << "[Engine] Renderer and ImGui initialized successfully.\n";

    // Load world static assets from JSON into core memory
    if (loadWorldStaticAssets("worldstaticassets.json", Core_GetWorldAssets()))
    {
        printWorldStaticAssets(Core_GetWorldAssets());
        LoadWorldTextures(Core_GetWorldAssets());
        std::cout << "[Engine] World assets loaded successfully.\n";
    }
    else
    {
        std::cerr << "[Engine Warning] Failed to load worldstaticassets.json or file missing.\n";
    }

    std::cout << "[Engine] Starting main loop...\n";

    // Main Engine Loop
    while (Renderer_IsRunning())
    {
        float deltaTime = Renderer_GetDeltaTime();

        // 1. Core Logic Tick
        Core_Update(deltaTime);

        // 2. Handle State Transitions
        if (Core_GetState() == GameState::Login)
        {
            if (UI_IsConnectPressed())
            {
                Core_GetWorld().spawnWorld();
            }
        }

        // 3. Render Pass
        Renderer_BeginFrame();
        Renderer_EndFrame();
    }

    std::cout << "[Engine] Shutting down clean...\n";
    Renderer_Shutdown();

    return 0;
}