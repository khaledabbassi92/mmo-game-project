#include "core.h"
#include <iostream>

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

    // 1. Draw all static world assets loaded from JSON
    Render_WorldStaticAssets(worldStaticAssets);

    // 2. Render dynamic entities (Player, Mobs, etc.)
    MainPlayer_Render();
}

// Iterates over all parsed assets and submits GPU draw calls
void Render_WorldStaticAssets(const WorldStaticAssets& assets)
{
    for (std::size_t i = 0; i < assets.count; ++i)
    {
        // Skip rendering if no texture was loaded to VRAM for this asset
        if (assets.textureID[i] == 0) continue;

        // Render quad at position (x, y) with scale
        Render_DrawSprite(
            assets.textureID[i],
            assets.position[i].x,
            assets.position[i].y,
            assets.scale[i]
        );
    }
}