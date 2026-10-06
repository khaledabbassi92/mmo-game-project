#pragma once
#include <string>
#include <array>
#include <cstddef>

//constants
constexpr std::size_t MAX_ASSETS = 1000;
constexpr std::size_t MAX_FLAGS_PER_ASSET = 8;

enum class GameState {
    Login,
    Playing,
    Options
};

struct Vector2 {
    float x;
    float y;
};

struct WorldStaticAssets {
    std::array<Vector2, MAX_ASSETS> position;
    std::array<float, MAX_ASSETS> scale;
    std::array<int, MAX_ASSETS> meshId;
	
    std::array<std::array<int, MAX_FLAGS_PER_ASSET>, MAX_ASSETS> flags;
    std::array<std::size_t, MAX_ASSETS> flagCount;
	
    std::array<int, MAX_ASSETS> type;
    std::size_t count = 0; 
};

struct WorldEnemies {
	
};

struct World {
    bool world_isSpawned = false;
    void spawnWorld();
    void despawnWorld();
    void renderWorld();
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

// UI State Structure Definition
struct UIState
{
    char username[128] = "";
    char password[128] = "";
    bool showOptions = false;
    bool connectPressed = false;
};

// NETWORK

bool SendLoginRequest();

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
extern WorldStaticAssets worldStaticAssets;
extern MainPlayer mainPlayer;
extern UIState uiState;

void Core_Update(float deltaTime);
GameState Core_GetState();
void Core_SetState(GameState state);

World& Core_GetWorld();
Camera& Core_GetCamera();
Players& Core_GetPlayers();
Mobs& Core_GetMobs();
WorldStaticAssets& Core_GetWorldStaticAssets();
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

// Assets
bool loadWorldStaticAssets(const std::string& filePath, WorldStaticAssets& outAssets);
void printWorldStaticAssets(const WorldStaticAssets& assets);