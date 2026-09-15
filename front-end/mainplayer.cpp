#include "core.h"
#include <imgui.h>
#include <SDL3/SDL.h>

static MainPlayer mainplayer = {
    1, "Hero", {0.0f, 0.0f}, 1, 100, 100, 0, 0, true
};

MainPlayer& Core_GetMainPlayer() {
    return mainplayer;
}

void MainPlayer_Update(float deltaTime) {
    const bool* keystate = SDL_GetKeyboardState(NULL);
    float speed = 200.0f * deltaTime;

    if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_UP])    mainplayer.position.y -= speed;
    if (keystate[SDL_SCANCODE_S] || keystate[SDL_SCANCODE_DOWN])  mainplayer.position.y += speed;
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT])  mainplayer.position.x -= speed;
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) mainplayer.position.x += speed;
}

void MainPlayer_Render() {
    Camera& cam = Core_GetCamera();
    
    float screenX = (mainplayer.position.x - cam.position.x) + (cam.width * 0.5f);
    float screenY = (mainplayer.position.y - cam.position.y) + (cam.height * 0.5f);

    ImGui::GetForegroundDrawList()->AddCircleFilled(
        ImVec2(screenX, screenY), 
        10.0f * cam.zoom, 
        IM_COL32(0, 255, 100, 255)
    );
}