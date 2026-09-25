#pragma once
#include <string>

enum class GameState {
    Login,
    Playing,
    Options
};

struct World {
    bool world_isSpawned = false;
    void spawnWorld();
    void despawnWorld();
    void renderWorld();
};

struct Vector2 {
    float x;
    float y;
};

struct Camera {
    Vector2 position;
    int width;
    int height;
    float zoom;
};

struct Players {};

struct MainPlayer {
    int id;
    std::string username;
    Vector2 position;
    int level;
    int health;
    int maxHealth;
    int experience;
    int animation;
    bool connected;
};

struct Mobs {};
struct WorldAssets {};

// NETWORK

bool Addplayertoserver();

bool TCP_Connect();
void TCP_Disconnect();
bool SendMainPlayerObject();

bool UDP_Connect();
void UDP_Disconnect();

void UpdatePlayerPosition(float deltaTime);

// Direct global variables (extern declaration for cross-file access)
extern GameState current_state;
extern World world;
extern Camera camera;
extern Players players;
extern Mobs mobs;
extern WorldAssets worldAssets;
extern MainPlayer mainPlayer;

void Core_Update(float deltaTime);
GameState Core_GetState();
void Core_SetState(GameState state);

World& Core_GetWorld();
Camera& Core_GetCamera();
Players& Core_GetPlayers();
Mobs& Core_GetMobs();
WorldAssets& Core_GetWorldAssets();
MainPlayer& Core_GetMainPlayer();

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

// Player & Camera Declarations
void Camera_Update(float deltaTime);
void MainPlayer_Update(float deltaTime);
void MainPlayer_Render();