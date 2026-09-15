#include "core.h"

void Camera_Update(float deltaTime) {
    Camera& cam = Core_GetCamera();
    MainPlayer& player = Core_GetMainPlayer();

    cam.position.x = player.position.x;
    cam.position.y = player.position.y;
}