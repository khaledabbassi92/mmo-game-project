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
            // 1. Establish sockets
            if ( 1 == 2)
            {
                std::cout << "Connect error" << std::endl;
            }
            // 2. Send credentials & wait for handshake
            else if (!SendLoginRequest())
            {
                std::cout << "Authenticated. Spawning world." << std::endl;
                loadWorldStaticAssets("worldstaticassets.json", worldStaticAssets);
                printWorldStaticAssets(worldStaticAssets);
                
                world.spawnWorld();
				
                std::cout << "Current state: " << static_cast<int>(current_state) << std::endl;
            }
            // 3. Handle auth failure
            else
            {
                std::cerr << "Authentication failed. Staying on login." << std::endl;
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