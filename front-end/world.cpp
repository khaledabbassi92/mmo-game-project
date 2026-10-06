#include "core.h"
#include <iostream>
#include <vector>
#include <algorithm>

void World::spawnWorld()
{
    world_isSpawned = true;
    current_state = GameState::Playing;
    std::cout << "[World] World spawned! Entering gameplay state.\n";
}

void World::despawnWorld()
{
    world_isSpawned = false;
    current_state = GameState::Login;
    std::cout << "[World] World despawned! Returning to login screen.\n";
}

void World::renderWorld()
{
    if (!world_isSpawned) return;

    // 1. Submit Static World Assets
    Render_WorldStaticAssets(worldStaticAssets);

    // 2. Submit Player & Dynamic Entities
    MainPlayer_Render();
}

void Render_WorldStaticAssets(const WorldStaticAssets& assets)
{
    for (std::size_t i = 0; i < assets.count; ++i)
    {
        Render_DrawSprite(
            assets.textureID[i],
            assets.position[i].x,
            assets.position[i].y,
            assets.scale[i]
        );
    }
}