#ifndef CORE_H
#define CORE_H

#include <string>
#include <vector>
#include <cstddef>
#include <unordered_map>
#include <cstdint>

#define MAX_ASSETS 1000
#define MAX_FLAGS_PER_ASSET 8

// -----------------------------------------------------------------------------
// CORE ENUMS & BASIC TYPES
// -----------------------------------------------------------------------------

enum class GameState {
    Login,
    Playing,
    Options
};

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
};

// Alias struct for inline nested coordinates compatibility
using Vec2 = Vector2;

// -----------------------------------------------------------------------------
// DATA CONTAINERS
// -----------------------------------------------------------------------------

struct MainPlayer {
    int id = 1;
    char name[32] = "Player1";
    Vector2 position = { 0.0f, 0.0f };
    int level = 1;
    int health = 100;
    int maxHealth = 100;
    unsigned int textureID = 0;
    float scale = 1.0f;
    bool isAlive = true;

    void update(float deltaTime);
};

struct Mob {
    int id = 0;
    char name[64] = "Mob";
    Vector2 position = { 0.0f, 0.0f };
    int level = 1;
    int health = 50;
    int maxHealth = 50;
    bool isAlive = true;
};

using Player  = MainPlayer;
using Players = std::vector<Player>;
using Mobs    = std::vector<Mob>;

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

// Legacy UI State Alias
struct UIState {
    char username[128] = "Player1";
    char password[128] = "";
    bool connectPressed = false;
    bool showOptions = false;
};

// -----------------------------------------------------------------------------
// SUBSYSTEM DECLARATIONS
// -----------------------------------------------------------------------------

struct Camera {
    Vector2 position = { 0.0f, 0.0f };
    float width = 1280.0f;
    float height = 720.0f;
    float zoom = 1.0f;

    void update(float targetX, float targetY, float deltaTime);
    void follow(const MainPlayer& player, float deltaTime);
};

struct World {
    bool world_isSpawned = false;

    void spawnWorld(GameState& outState);
    void despawnWorld(GameState& outState);
};

struct UIManager {
    char username[32] = "";
    char password[32] = "";
    bool connectPressed = false;

    bool isConnectPressed();
    void resetInputs();
};

struct AssetManager {
    std::unordered_map<std::string, unsigned int> textureCache;

    unsigned int loadTexture(const std::string& filePath);
    void loadWorldTextures(WorldStaticAssets& assets);
    bool loadWorldStaticAssets(const std::string& filePath, WorldStaticAssets& outAssets);
    void unloadAll();
    void printWorldStaticAssets(const WorldStaticAssets& assets) const;
};

struct NetworkManager {
    uintptr_t tcpSocket = ~0ULL; // INVALID_SOCKET equivalent
    uintptr_t udpSocket = ~0ULL;
    bool winsockInitialized = false;

    bool init();
    void shutdown();

    bool connectTCP(const char* ip = "127.0.0.1", unsigned short port = 4444);
    bool sendTCP(const char* data, int size);
    int receiveTCP(char* buffer, int size);
    void disconnectTCP();

    bool connectUDP(const char* ip = "127.0.0.1", unsigned short port = 4445);
    bool sendUDP(const char* data, int size);
    int receiveUDP(char* buffer, int size);
    void disconnectUDP();

    bool sendLoginRequest(const UIManager& ui);
    void sendPlayerPosition(const MainPlayer& player, float deltaTime);
};

// -----------------------------------------------------------------------------
// CENTRAL GAME CONTEXT DECLARATION
// -----------------------------------------------------------------------------

struct GameContext {
    GameState state = GameState::Login;

    World world;
    Camera camera = { {0.0f, 0.0f}, 1280.0f, 720.0f, 1.0f };
    Players players;
    Mobs mobs;
    WorldStaticAssets worldStaticAssets;
    MainPlayer mainPlayer = { 1, "Player1", {0.0f, 0.0f}, 1, 100, 100, 0, 0, true };
    UIManager ui;
    AssetManager assets;
    NetworkManager net;

    void update(float deltaTime);
};

// -----------------------------------------------------------------------------
// RENDERER DECLARATION
// -----------------------------------------------------------------------------

struct Renderer {
    void* window = nullptr;
    void* gl_context = nullptr;
    uint64_t last_counter = 0;
    float delta_time = 0.0f;
    bool app_running = true;

    bool init(int width, int height, const char* title);
    bool isRunning(Camera& camera);
    void shutdown();
    float getDeltaTime() const { return delta_time; }

    void beginFrame(Camera& camera);
    void endFrame();

    // 2D Drawing Operations
    void drawSprite(unsigned int textureID, float worldX, float worldY, float scale, const Camera& camera);
    void renderWorld(const WorldStaticAssets& assets, const Camera& camera);
    void renderPlayer(const MainPlayer& player, const Camera& camera);
    
    // Combined Scene Render (World + Player)
    void renderScene(const WorldStaticAssets& assets, const MainPlayer& player, const Camera& camera);

    // UI Overlays
    void drawLoginWindow(UIManager& ui);
    void drawHUD(const MainPlayer& player, const Camera& camera, const WorldStaticAssets& assets);
};

#endif // CORE_H