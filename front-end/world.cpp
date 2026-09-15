#include "core.h"
#include <imgui.h>
#include <SDL3/SDL.h>

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
        MainPlayer_Update(deltaTime);
        Camera_Update(deltaTime);
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

    Camera& cam = Core_GetCamera();
    cam.width = 900;
    cam.height = 600;
    if (cam.zoom <= 0.0f) cam.zoom = 1.0f;

    ImGui::Begin("World Simulation");
    ImGui::Text("Player Pos: (%.1f, %.1f)", Core_GetMainPlayer().position.x, Core_GetMainPlayer().position.y);
    
    if (ImGui::Button("Disconnect / Return to Login"))
    {
        despawnWorld();
    }

    ImGui::End();

    MainPlayer_Render();
}