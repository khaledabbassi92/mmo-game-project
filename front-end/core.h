// ============================================================================
// core.h - Sole Intermediary Header (SDL3 + OpenGL3)
// ============================================================================
#pragma once

#include <SDL3/SDL.h>

// --- GUI Subsystem (implemented in gui.cpp) ---
bool GUI_Init(int width = 1280, int height = 720, const char* title = "MMO Client - 2D World (SDL3/OpenGL3)");
bool GUI_IsRunning();
void GUI_BeginFrame();
void GUI_EndFrame();
void GUI_Shutdown();

float GUI_GetDeltaTime();
void GUI_GetWindowSize(int* w, int* h);
const bool* GUI_GetKeyboardState();
bool GUI_IsConnectPressed();

// --- World Subsystem (implemented in world.cpp) ---
void World_Spawn(float spawnX = 1000.0f, float spawnY = 1000.0f);
void World_Despawn();
bool World_IsSpawned();
void World_Update(float deltaTime);
void World_Render();