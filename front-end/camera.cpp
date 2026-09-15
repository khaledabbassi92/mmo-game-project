#include "core.h"

void Camera_Update(float deltaTime) {
    camera.position.x = mainPlayer.position.x;
    camera.position.y = mainPlayer.position.y;
}