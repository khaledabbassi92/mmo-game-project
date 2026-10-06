#include "core.h"
#include <imgui.h>
#include <SDL3/SDL.h>

void MainPlayer_Update(float deltaTime)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);
    float speed = 250.0f * deltaTime; // Movement speed in units/sec

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    mainPlayer.position.y -= speed;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  mainPlayer.position.y += speed;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  mainPlayer.position.x -= speed;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) mainPlayer.position.x += speed;
}

void MainPlayer_Render()
{
    // Convert world space coordinates to screen space based on camera position
    float screenX = mainPlayer.position.x - camera.position.x + (camera.width * 0.5f);
    float screenY = mainPlayer.position.y - camera.position.y + (camera.height * 0.5f);

    // Render player debug indicator via ImGui DrawList
    ImGui::GetForegroundDrawList()->AddCircleFilled(
        ImVec2(screenX, screenY),
        12.0f,
        IM_COL32(0, 255, 100, 255)
    );
}