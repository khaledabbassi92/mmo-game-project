#include "core.h"
#include <cstring>

bool UIManager::isConnectPressed()
{
    if (connectPressed)
    {
        connectPressed = false;
        return true;
    }
    return false;
}

void UIManager::resetInputs()
{
    std::memset(username, 0, sizeof(username));
    std::memset(password, 0, sizeof(password));
    connectPressed = false;
}