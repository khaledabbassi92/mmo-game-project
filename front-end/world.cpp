// ============================================================================
// world.cpp - Entire 2D World Logic (2000x2000 coordinates, clamping, camera)
// ============================================================================
#include "core.h"

#include <imgui.h>
#include <algorithm>
#include <cmath>

namespace {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }

    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float l = length();
        return (l > 0.0001f) ? Vec2{x / l, y / l} : Vec2{0.0f, 0.0f};
    }
};

// 2000x2000 World Coordinates
constexpr float MIN_X      = 0.0f;
constexpr float MIN_Y      = 0.0f;
constexpr float MAX_X      = 2000.0f;
constexpr float MAX_Y      = 2000.0f;
constexpr float MINOR_GRID = 50.0f;
constexpr float MAJOR_GRID = 250.0f;

bool s_isSpawned = false;

struct {
    Vec2  position{1000.0f, 1000.0f};
    Vec2  velocity{0.0f, 0.0f};
    float speed = 350.0f;
    float radius = 16.0f;
    float facingAngle = 0.0f;
} s_player;

struct {
    Vec2  position{1000.0f, 1000.0f};
    float zoom = 1.0f;
    bool  followPlayer = true;
    float lerpSpeed = 10.0f;
} s_camera;

Vec2 WorldToScreen(const Vec2& worldPos, int screenW, int screenH) {
    float sx = (worldPos.x - s_camera.position.x) * s_camera.zoom + (screenW * 0.5f);
    float sy = (worldPos.y - s_camera.position.y) * s_camera.zoom + (screenH * 0.5f);
    return {sx, sy};
}

Vec2 ScreenToWorld(const Vec2& screenPos, int screenW, int screenH) {
    float wx = (screenPos.x - (screenW * 0.5f)) / s_camera.zoom + s_camera.position.x;
    float wy = (screenPos.y - (screenH * 0.5f)) / s_camera.zoom + s_camera.position.y;
    return {wx, wy};
}

Vec2 ClampToBounds(const Vec2& pos, float radius) {
    Vec2 res = pos;
    res.x = std::clamp(res.x, MIN_X + radius, MAX_X - radius);
    res.y = std::clamp(res.y, MIN_Y + radius, MAX_Y - radius);
    return res;
}

} // anonymous namespace

void World_Spawn(float spawnX, float spawnY)
{
    s_player.position = ClampToBounds({spawnX, spawnY}, s_player.radius);
    s_player.velocity = {0.0f, 0.0f};
    s_camera.position = s_player.position;
    s_isSpawned = true;
}

void World_Despawn()
{
    s_isSpawned = false;
}

bool World_IsSpawned()
{
    return s_isSpawned;
}

void World_Update(float deltaTime)
{
    if (!s_isSpawned) return;

    // In SDL3, keyboard state returns const bool* indexed by SDL_Scancode
    const bool* keys = GUI_GetKeyboardState();
    Vec2 dir{0.0f, 0.0f};
    if (keys) {
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    dir.y -= 1.0f;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  dir.y += 1.0f;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  dir.x -= 1.0f;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) dir.x += 1.0f;
    }

    Vec2 norm = dir.normalized();
    s_player.velocity = norm * s_player.speed;
    if (norm.x != 0.0f || norm.y != 0.0f) {
        s_player.facingAngle = std::atan2(norm.y, norm.x);
    }

    // Boundary Clamping inside [0, 2000]
    Vec2 nextPos = s_player.position + s_player.velocity * deltaTime;
    s_player.position = ClampToBounds(nextPos, s_player.radius);

    // Camera follow
    if (s_camera.followPlayer) {
        float alpha = 1.0f - std::exp(-s_camera.lerpSpeed * deltaTime);
        s_camera.position.x += (s_player.position.x - s_camera.position.x) * alpha;
        s_camera.position.y += (s_player.position.y - s_camera.position.y) * alpha;
    }
}

void World_Render()
{
    if (!s_isSpawned) return;

    int screenW, screenH;
    GUI_GetWindowSize(&screenW, &screenH);

    // Use Dear ImGui's Background DrawList (rendered behind all ImGui windows)
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // 1. Draw Flat Terrain (2000x2000)
    Vec2 tl = WorldToScreen({MIN_X, MIN_Y}, screenW, screenH);
    Vec2 br = WorldToScreen({MAX_X, MAX_Y}, screenW, screenH);
    drawList->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(br.x, br.y), IM_COL32(24, 30, 41, 255));

    // Visible bounding box in world space
    Vec2 vTL = ScreenToWorld({0.0f, 0.0f}, screenW, screenH);
    Vec2 vBR = ScreenToWorld({(float)screenW, (float)screenH}, screenW, screenH);
    float viewMinX = std::max(MIN_X, vTL.x);
    float viewMinY = std::max(MIN_Y, vTL.y);
    float viewMaxX = std::min(MAX_X, vBR.x);
    float viewMaxY = std::min(MAX_Y, vBR.y);

    // 2. Minor Grid (50u)
    float startX = std::floor(viewMinX / MINOR_GRID) * MINOR_GRID;
    float startY = std::floor(viewMinY / MINOR_GRID) * MINOR_GRID;
    for (float x = startX; x <= viewMaxX; x += MINOR_GRID) {
        Vec2 p1 = WorldToScreen({x, std::max(MIN_Y, viewMinY)}, screenW, screenH);
        Vec2 p2 = WorldToScreen({x, std::min(MAX_Y, viewMaxY)}, screenW, screenH);
        drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), IM_COL32(34, 45, 61, 255), 1.0f);
    }
    for (float y = startY; y <= viewMaxY; y += MINOR_GRID) {
        Vec2 p1 = WorldToScreen({std::max(MIN_X, viewMinX), y}, screenW, screenH);
        Vec2 p2 = WorldToScreen({std::min(MAX_X, viewMaxX), y}, screenW, screenH);
        drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), IM_COL32(34, 45, 61, 255), 1.0f);
    }

    // 3. Major Grid (250u)
    float majStartX = std::floor(viewMinX / MAJOR_GRID) * MAJOR_GRID;
    float majStartY = std::floor(viewMinY / MAJOR_GRID) * MAJOR_GRID;
    for (float x = majStartX; x <= viewMaxX; x += MAJOR_GRID) {
        Vec2 p1 = WorldToScreen({x, std::max(MIN_Y, viewMinY)}, screenW, screenH);
        Vec2 p2 = WorldToScreen({x, std::min(MAX_Y, viewMaxY)}, screenW, screenH);
        drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), IM_COL32(51, 65, 85, 255), 1.5f);
    }
    for (float y = majStartY; y <= viewMaxY; y += MAJOR_GRID) {
        Vec2 p1 = WorldToScreen({std::max(MIN_X, viewMinX), y}, screenW, screenH);
        Vec2 p2 = WorldToScreen({std::min(MAX_X, viewMaxX), y}, screenW, screenH);
        drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), IM_COL32(51, 65, 85, 255), 1.5f);
    }

    // 4. Outer Boundaries (Hazard Red)
    drawList->AddRect(ImVec2(tl.x, tl.y), ImVec2(br.x, br.y), IM_COL32(239, 68, 68, 255), 0.0f, 0, 3.0f);

    // 5. Draw Player Avatar
    Vec2 pScreen = WorldToScreen(s_player.position, screenW, screenH);
    float pScreenRad = s_player.radius * s_camera.zoom;
    drawList->AddCircleFilled(ImVec2(pScreen.x, pScreen.y), pScreenRad, IM_COL32(56, 189, 248, 255));

    // Player Direction Line
    float endX = pScreen.x + std::cos(s_player.facingAngle) * (pScreenRad + 10.0f);
    float endY = pScreen.y + std::sin(s_player.facingAngle) * (pScreenRad + 10.0f);
    drawList->AddLine(ImVec2(pScreen.x, pScreen.y), ImVec2(endX, endY), IM_COL32(255, 255, 255, 255), 2.0f);

    // 6. World Inspector HUD
    ImGui::Begin("World Space Inspector");
    ImGui::Text("Player World Pos: (%.2f, %.2f)", s_player.position.x, s_player.position.y);
    ImGui::Text("Player Velocity: (%.1f, %.1f) u/s", s_player.velocity.x, s_player.velocity.y);
    ImGui::Text("Playable Bounds: [0.0, 2000.0]");
    ImGui::SliderFloat("Move Speed", &s_player.speed, 100.0f, 800.0f);
    ImGui::SliderFloat("Camera Zoom", &s_camera.zoom, 0.2f, 3.0f);
    if (ImGui::Button("Respawn at Center (1000, 1000)")) {
        World_Spawn(1000.0f, 1000.0f);
    }
    ImGui::End();
}