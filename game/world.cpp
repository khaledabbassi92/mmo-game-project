#include "core.h"
#include <iostream>

void World::spawnWorld(GameState& outState)
{
    world_isSpawned = true;
    outState = GameState::Playing;
    std::cout << "[World] World spawned! Entering gameplay state.\n";
}

void World::despawnWorld(GameState& outState)
{
    world_isSpawned = false;
    outState = GameState::Login;
    std::cout << "[World] World despawned! Returning to login screen.\n";
}