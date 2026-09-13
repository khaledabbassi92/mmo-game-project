#include "core.h"
#include <imgui.h>

static GameState current_state = GameState::Login;
static World world;
static Camera camera;
static Players players;
static Mobs mobs;
static WorldAssets worldAssets;

GameState Core_GetState() { return current_state; }
void Core_SetState(GameState state) { current_state = state; }

World& Core_GetWorld() { return world; }
Camera& Core_GetCamera() { return camera; }
Players& Core_GetPlayers() { return players; }
Mobs& Core_GetMobs() { return mobs; }
WorldAssets& Core_GetWorldAssets() { return worldAssets; }

void Core_Update(float deltaTime)
{
    if (current_state == GameState::Playing)
    {
        // Update simulation systems here
    }
}

void World::spawnWorld()
{
    if (world_isSpawned) return;
    world_isSpawned = true;
    Core_SetState(GameState::Playing);
}

void World::despawnWorld()
{
    if (!world_isSpawned) return;
    world_isSpawned = false;
    Core_SetState(GameState::Login);
}

void World::renderWorld()
{
    if (current_state != GameState::Playing) return;

    ImGui::Begin("World Simulation");
    ImGui::Text("Active Simulation Space");
    
    if (ImGui::Button("Disconnect / Return to Login"))
    {
        despawnWorld();
    }

    ImGui::End();
}