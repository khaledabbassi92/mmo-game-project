#include "core.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <iostream>

void World::spawnWorld()
{
    if (world_isSpawned) return;
    world_isSpawned = true;
    current_state = GameState::Playing;
}

void World::despawnWorld()
{
    if (!world_isSpawned) return;
    world_isSpawned = false;
    current_state = GameState::Login;
}

void World::renderWorld()
{
    if (current_state != GameState::Playing) return;

    camera.width = 900;
    camera.height = 600;
    if (camera.zoom <= 0.0f) camera.zoom = 1.0f;

    ImGui::Begin("World Simulation");
    ImGui::Text("Player Pos: (%.1f, %.1f)", mainPlayer.position.x, mainPlayer.position.y);
    
    if (ImGui::Button("Disconnect / Return to Login"))
    {
        despawnWorld();
		std::cout <<"Currnet state: " << static_cast<int>(current_state) << std::endl;
    }

    ImGui::End();

    MainPlayer_Render();
}