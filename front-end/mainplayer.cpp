#include "core.h"
#include <imgui.h>
#include <SDL3/SDL.h>

void MainPlayer_Update(float deltaTime) {
    const bool* keystate = SDL_GetKeyboardState(NULL);
    float speed = 200.0f * deltaTime;

    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])    mainPlayer.position.y -= speed;
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN])  mainPlayer.position.y += speed;
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])  mainPlayer.position.x -= speed;
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) mainPlayer.position.x += speed;
}

void MainPlayer_Render() {
    float screenX = (mainPlayer.position.x - camera.position.x) + (camera.width * 0.5f);
    float screenY = (mainPlayer.position.y - camera.position.y) + (camera.height * 0.5f);

    ImGui::GetForegroundDrawList()->AddCircleFilled(
        ImVec2(screenX, screenY), 
        10.0f * camera.zoom, 
        IM_COL32(0, 255, 100, 255)
    );
}