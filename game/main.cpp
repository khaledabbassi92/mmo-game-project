#include "core.h"
#include <iostream>
#include <SDL3/SDL.h>

int main(int argc, char* argv[])
{
    Renderer renderer;
    GameContext game;

    if (!renderer.init(1280, 720, "2D MMO Engine"))
    {
        std::cerr << "[Fatal Error] Failed to initialize rendering context." << std::endl;
        return -1;
    }

    if (game.assets.loadWorldStaticAssets("worldstaticassets.json", game.worldStaticAssets))
    {
        game.assets.loadWorldTextures(game.worldStaticAssets);
    }

    while (renderer.isRunning(game.camera))
    {
        float deltaTime = renderer.getDeltaTime();

        // 1. STATE MACHINE & LOGIC UPDATE
        switch (game.state)
        {
            case GameState::Login:
            {
                if (game.ui.isConnectPressed())
                {
                    game.world.spawnWorld(game.state); // Changes state to Playing
                }
                break;
            }
            case GameState::Playing:
            {
                // Check if ESCAPE key is pressed to exit gameplay back to Login
                const bool* keyState = SDL_GetKeyboardState(NULL);
                if (keyState[SDL_SCANCODE_ESCAPE])
                {
                    game.world.despawnWorld(game.state); // Despawns world & sets state to Login
                }
                else
                {
                    game.update(deltaTime);
                }
                break;
            }
            case GameState::Options:
            {
                break;
            }
        }

        // 2. RENDERING PASS
        renderer.beginFrame(game.camera);

        switch (game.state)
        {
            case GameState::Login:
            {
                renderer.drawLoginWindow(game.ui);
                break;
            }
            case GameState::Playing:
            {
                if (game.world.world_isSpawned)
                {
                    renderer.renderScene(game.worldStaticAssets, game.mainPlayer, game.camera);
                }
                renderer.drawHUD(game.mainPlayer, game.camera, game.worldStaticAssets);
                break;
            }
            case GameState::Options:
            {
                break;
            }
        }

        renderer.endFrame();
    }

    game.assets.unloadAll();
    game.net.shutdown();
    renderer.shutdown();

    return 0;
}