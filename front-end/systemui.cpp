#include "core.h"
#include <iostream>
#include <cstring>
#include "imgui.h"

UIState uiState;

void DrawLoginWindow()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("LoginRoot", nullptr, flags);

    const float fieldWidth = 260.0f;
    const float buttonWidth = 200.0f;
    const float buttonHeight = 34.0f;
    const float spacing = 10.0f;
    const float groupGap = 24.0f;

    const float totalHeight =
        ImGui::GetFrameHeightWithSpacing() * 2.0f +
        groupGap +
        buttonHeight * 3.0f +
        spacing * 2.0f;

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float startY = (avail.y - totalHeight) * 0.5f;
    if (startY < 0.0f) startY = 0.0f;

    auto CenterNextWidget = [&](float width)
    {
        float x = (avail.x - width) * 0.5f;
        if (x < 0.0f) x = 0.0f;
        ImGui::SetCursorPosX(x);
    };

    ImGui::SetCursorPosY(startY);

    CenterNextWidget(fieldWidth);
    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::InputTextWithHint("##username", "Username", uiState.username, IM_ARRAYSIZE(uiState.username));
    ImGui::Dummy(ImVec2(0.0f, spacing * 0.5f));

    CenterNextWidget(fieldWidth);
    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::InputTextWithHint("##password", "Password", uiState.password, IM_ARRAYSIZE(uiState.password), ImGuiInputTextFlags_Password);
    ImGui::Dummy(ImVec2(0.0f, groupGap));

    CenterNextWidget(buttonWidth);
    if (ImGui::Button("Connect", ImVec2(buttonWidth, buttonHeight)))
    {
        uiState.connectPressed = true;
        std::cout << "[Connect] username=\"" << uiState.username << "\"" << std::endl;
    }

    ImGui::Dummy(ImVec2(0.0f, spacing));

    CenterNextWidget(buttonWidth);
    if (ImGui::Button("Options", ImVec2(buttonWidth, buttonHeight)))
    {
        uiState.showOptions = true;
    }

    ImGui::Dummy(ImVec2(0.0f, spacing));

    CenterNextWidget(buttonWidth);
    if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight)))
    {
        std::exit(0);
    }

    ImGui::End();

    if (uiState.showOptions)
    {
        ImGui::SetNextWindowSize(ImVec2(300.0f, 150.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Options", &uiState.showOptions);
        ImGui::Text("Put your game options here.");
        ImGui::End();
    }
}

bool UI_IsConnectPressed()
{
    bool pressed = uiState.connectPressed;
    uiState.connectPressed = false;
    return pressed;
}