#include "core.h"

GameState current_state = GameState::Login;
World world;
Camera camera = { {0.0f, 0.0f}, 1280, 720, 1.0f };
Players players;
Mobs mobs;
WorldAssets worldAssets;
MainPlayer mainPlayer = { 1, "Player1", {0.0f, 0.0f}, 1, 100, 100, 0, 0, false };

GameState Core_GetState() { return current_state; }
void Core_SetState(GameState state) { current_state = state; }

World& Core_GetWorld() { return world; }
Camera& Core_GetCamera() { return camera; }
Players& Core_GetPlayers() { return players; }
Mobs& Core_GetMobs() { return mobs; }
WorldAssets& Core_GetWorldAssets() { return worldAssets; }
MainPlayer& Core_GetMainPlayer() { return mainPlayer; }

void Core_Update(float deltaTime)
{
    if (current_state == GameState::Playing)
    {
        MainPlayer_Update(deltaTime);
        Camera_Update(deltaTime);
    }
}