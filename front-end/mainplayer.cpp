#include "core.h"
#include <SDL3/SDL.h>

void MainPlayer::update(float deltaTime)
{
    if (!isAlive) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);
    float speed = 250.0f * deltaTime;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    position.y -= speed;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  position.y += speed;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  position.x -= speed;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) position.x += speed;
}