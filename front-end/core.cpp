#include "core.h"

// -----------------------------------------------------------------------------
// GLOBAL GAME OBJECT CONSTRUCTORS
// -----------------------------------------------------------------------------
GameState current_state = GameState::Login;
World world;
Camera camera = { {0.0f, 0.0f}, 1280.0f, 720.0f, 1.0f };
Players players;
Mobs mobs;
WorldStaticAssets worldStaticAssets;
MainPlayer mainPlayer = { 1, "Player1", {0.0f, 0.0f}, 1, 100, 100, 0, 0, true };
UIState uiState;

// -----------------------------------------------------------------------------
// GETTERS & SETTERS
// -----------------------------------------------------------------------------
GameState Core_GetState() { return current_state; }
void Core_SetState(GameState state) { current_state = state; }

World& Core_GetWorld() { return world; }
Camera& Core_GetCamera() { return camera; }
Players& Core_GetPlayers() { return players; }
Mobs& Core_GetMobs() { return mobs; }
WorldStaticAssets& Core_GetWorldAssets() { return worldStaticAssets; }
MainPlayer& Core_GetMainPlayer() { return mainPlayer; }
UIState& Core_GetUIState() { return uiState; }

// -----------------------------------------------------------------------------
// CORE TICK UPDATE
// -----------------------------------------------------------------------------
void Core_Update(float deltaTime)
{
    if (current_state == GameState::Playing)
    {
        MainPlayer_Update(deltaTime);
        Camera_Update(deltaTime);
    }
}