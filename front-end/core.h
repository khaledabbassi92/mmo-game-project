#ifndef CORE_H
#define CORE_H

#include <string>
#include <cstddef>
#include <vector>

#define MAX_ASSETS 1000
#define MAX_FLAGS_PER_ASSET 8

enum class GameState {
    Login,
    Playing,
    Options
};

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Camera {
    Vector2 position;
    float width;
    float height;
    float zoom;
};

struct Player {
    int id;
    char name[64];
    Vector2 position;
    int level;
    int health;
    int maxHealth;
    int exp;
    int gold;
    bool isAlive;
};

using MainPlayer = Player;

struct Players {
    std::vector<Player> list;
};

struct Mob {
    int id;
    Vector2 position;
    int health;
};

struct Mobs {
    std::vector<Mob> list;
};

struct WorldStaticAssets {
    Vector2 position[MAX_ASSETS];
    float scale[MAX_ASSETS];
    int meshId[MAX_ASSETS];
    int type[MAX_ASSETS];
    std::string texturePath[MAX_ASSETS];
    unsigned int textureID[MAX_ASSETS];
    int flags[MAX_ASSETS][MAX_FLAGS_PER_ASSET];
    std::size_t flagCount[MAX_ASSETS];
    std::size_t count = 0;
};

class World {
public:
    bool world_isSpawned = false;
    void spawnWorld();
    void despawnWorld();
    void renderWorld();
};

struct UIState {
    char username[128] = "";
    char password[128] = "";
    bool connectPressed = false;
    bool showOptions = false;
};

// Extern Declarations (Tells other files these objects exist in core.cpp)
extern GameState current_state;
extern Camera camera;
extern Player mainPlayer;
extern Players players;
extern Mobs mobs;
extern World world;
extern WorldStaticAssets worldStaticAssets;
extern UIState uiState;

// Core Getters & Setters
GameState Core_GetState();
void Core_SetState(GameState state);
World& Core_GetWorld();
Camera& Core_GetCamera();
Players& Core_GetPlayers();
Mobs& Core_GetMobs();
WorldStaticAssets& Core_GetWorldAssets();
MainPlayer& Core_GetMainPlayer();
UIState& Core_GetUIState();
void Core_Update(float deltaTime);

// Renderer & Utility Declarations
bool Renderer_Init(int width, int height, const char* title);
bool Renderer_IsRunning();
void Renderer_BeginFrame();
void Renderer_EndFrame();
void Renderer_Shutdown();
void DrawHUD();
float Renderer_GetDeltaTime();

bool loadWorldStaticAssets(const std::string& filePath, WorldStaticAssets& outAssets);
void printWorldStaticAssets(const WorldStaticAssets& assets);
void LoadWorldTextures(WorldStaticAssets& assets);

void Render_DrawSprite(unsigned int textureID, float x, float y, float scale);
void Render_WorldStaticAssets(const WorldStaticAssets& assets);

void MainPlayer_Update(float deltaTime);
void MainPlayer_Render();
void DrawLoginWindow();
bool UI_IsConnectPressed();
void Camera_Update(float deltaTime);

#endif // CORE_H