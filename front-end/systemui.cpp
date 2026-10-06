#include "core.h"
#include <imgui.h>

void DrawLoginWindow()
{
    ImGui::SetNextWindowPos(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::InputText("Username", uiState.username, sizeof(uiState.username));
    ImGui::InputText("Password", uiState.password, sizeof(uiState.password), ImGuiInputTextFlags_Password);

    if (ImGui::Button("Connect"))
    {
        uiState.connectPressed = true;
    }

    ImGui::End();
}

void DrawHUD()
{
    // Configure overlay position (top-left, non-interactive background)
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f); // Semi-transparent overlay

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                             ImGuiWindowFlags_AlwaysAutoResize | 
                             ImGuiWindowFlags_NoSavedSettings | 
                             ImGuiWindowFlags_NoFocusOnAppearing | 
                             ImGuiWindowFlags_NoNav | 
                             ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Player Position HUD", nullptr, flags))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.4f, 1.0f), "PLAYER POSITION");
        ImGui::Separator();
        ImGui::Text("X: %.2f", mainPlayer.position.x);
        ImGui::Text("Y: %.2f", mainPlayer.position.y);

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "CAMERA");
        ImGui::Separator();
        ImGui::Text("Cam X: %.2f", camera.position.x);
        ImGui::Text("Cam Y: %.2f", camera.position.y);

        // Display static asset count from JSON
        ImGui::Spacing();
        ImGui::Text("World Static Assets: %zu", worldStaticAssets.count);
    }
    ImGui::End();
}

bool UI_IsConnectPressed()
{
    if (uiState.connectPressed)
    {
        uiState.connectPressed = false;
        return true;
    }
    return false;
}