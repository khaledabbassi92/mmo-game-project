#include "core.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (!Renderer_Init(900, 600, "MMO Game"))
    {
        return 1;
    }

    while (Renderer_IsRunning())
    {
        Renderer_BeginFrame();

        if (UI_IsConnectPressed())
        {
            std::cout << "[Main] Connect pressed. Spawning world." << std::endl;
            Core_GetWorld().spawnWorld();
        }

        Core_Update(Renderer_GetDeltaTime());
        Renderer_EndFrame();
    }

    Renderer_Shutdown();
    return 0;
}

