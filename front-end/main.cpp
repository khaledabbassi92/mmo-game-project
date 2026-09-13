// ============================================================================
// main.cpp - Pure Launcher
// ============================================================================
#include "core.h"

int main(int argc, char* argv[])
{
    if (!GUI_Init(1280, 720, "MMO Client - 2D World (2000x2000)")) {
        return -1;
    }

    while (GUI_IsRunning()) {
        GUI_BeginFrame();

        // Check when player clicks "Connect" to spawn the world
        if (GUI_IsConnectPressed()) {
            if (!World_IsSpawned()) {
                World_Spawn(1000.0f, 1000.0f); // Center of 2000x2000
            }
        }

        // Run simulation and draw world only when spawned
        if (World_IsSpawned()) {
            World_Update(GUI_GetDeltaTime());
            World_Render();
        }

        GUI_EndFrame();
    }

    GUI_Shutdown();
    return 0;
}