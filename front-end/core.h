#pragma once

enum class GameState {
    Login,
    Playing,
    Options
};

GameState Core_GetState();
void Core_SetState(GameState state);

struct World {
    bool world_isSpawned = false;
    void spawnWorld();
    void despawnWorld();
    void renderWorld();
};

struct Camera {};
struct Players {};
struct Mobs {};
struct WorldAssets {};

World& Core_GetWorld();
Camera& Core_GetCamera();
Players& Core_GetPlayers();
Mobs& Core_GetMobs();
WorldAssets& Core_GetWorldAssets();
void Core_Update(float deltaTime);

// Renderer / Window Function Declarations
bool Renderer_Init(int width, int height, const char* title);
bool Renderer_IsRunning();
void Renderer_BeginFrame();
void Renderer_EndFrame();
void Renderer_Shutdown();
float Renderer_GetDeltaTime();

// UI Function Declarations
void DrawLoginWindow();
bool UI_IsConnectPressed();