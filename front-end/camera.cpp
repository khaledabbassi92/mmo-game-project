#include "core.h"
#include <cmath>

void Camera::update(float targetX, float targetY, float deltaTime)
{
    float smoothing = 1.0f - std::exp(-8.0f * deltaTime);

    position.x += (targetX - position.x) * smoothing;
    position.y += (targetY - position.y) * smoothing;

    if (zoom <= 0.1f)
    {
        zoom = 1.0f;
    }
}

void Camera::follow(const MainPlayer& player, float deltaTime)
{
    update(player.position.x, player.position.y, deltaTime);
}