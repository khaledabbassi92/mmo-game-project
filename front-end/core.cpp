#include "core.h"

void GameContext::update(float deltaTime)
{
    if (state == GameState::Playing)
    {
        mainPlayer.update(deltaTime);
        camera.follow(mainPlayer, deltaTime);
    }
}