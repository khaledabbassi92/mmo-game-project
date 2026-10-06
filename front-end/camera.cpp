#include "core.h"
#include <cmath>

void Camera_Update(float deltaTime) 
{
    float smoothing = 1.0f - std::exp(-8.0f * deltaTime);
    camera.position.x += (mainPlayer.position.x - camera.position.x) * smoothing;
    camera.position.y += (mainPlayer.position.y - camera.position.y) * smoothing;
    
    if (camera.zoom <= 0.1f) camera.zoom = 1.0f;
}