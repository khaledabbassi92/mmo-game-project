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
            if (TCP_Connect() && UDP_Connect() && Addplayertoserver())
            {
                std::cout << "[Main] Registered. Spawning world." << std::endl;
                world.spawnWorld();
                std::cout << "Current state: " << static_cast<int>(current_state) << std::endl;
            }
            else
            {
                std::cerr << "[Main] Failed to connect/register. Staying on login." << std::endl;
                TCP_Disconnect();
                UDP_Disconnect();
            }
        }

        float deltaTime = Renderer_GetDeltaTime();

        Core_Update(deltaTime);
        UpdatePlayerPosition(deltaTime);

        Renderer_EndFrame();
    }

    TCP_Disconnect();
    UDP_Disconnect();
    Renderer_Shutdown();

    return 0;
}