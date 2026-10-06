#include "core.h"
#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

bool Renderer::init(int width, int height, const char* title)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "[SDL Error] Failed to initialize video: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_Window* wnd = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!wnd)
    {
        std::cerr << "[SDL Error] Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(wnd);
    if (!ctx)
    {
        std::cerr << "[SDL Error] Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_MakeCurrent(wnd, ctx);
    SDL_GL_SetSwapInterval(1); // VSync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(wnd, ctx);
    ImGui_ImplOpenGL3_Init("#version 120");

    window = (void*)wnd;
    gl_context = (void*)ctx;
    last_counter = SDL_GetPerformanceCounter();

    return true;
}

bool Renderer::isRunning(Camera& camera)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT ||
           (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && 
            event.window.windowID == SDL_GetWindowID((SDL_Window*)window)))
        {
            app_running = false;
        }

        if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            camera.width = (float)event.window.data1;
            camera.height = (float)event.window.data2;
        }
    }

    uint64_t current_counter = SDL_GetPerformanceCounter();
    uint64_t frequency = SDL_GetPerformanceFrequency();
    delta_time = static_cast<float>(current_counter - last_counter) / static_cast<float>(frequency);
    last_counter = current_counter;

    return app_running;
}

void Renderer::beginFrame(Camera& camera)
{
    glViewport(0, 0, (int)camera.width, (int)camera.height);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, camera.width, camera.height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer::drawSprite(unsigned int textureID, float worldX, float worldY, float scale, const Camera& camera)
{
    float screenX = (worldX - camera.position.x) * camera.zoom + (camera.width * 0.5f);
    float screenY = (worldY - camera.position.y) * camera.zoom + (camera.height * 0.5f);

    float baseW = 120.0f * scale * camera.zoom;
    float baseH = 160.0f * scale * camera.zoom;

    float left   = screenX - (baseW * 0.5f);
    float right  = screenX + (baseW * 0.5f);
    float bottom = screenY;
    float top    = screenY - baseH;

    // Screen frustum check
    if (right < 0 || left > camera.width || bottom < 0 || top > camera.height)
        return;

    if (textureID != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(left,  top);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(right, top);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(right, bottom);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(left,  bottom);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        // Placeholder tree rendering if no texture loaded
        glColor4f(0.3f, 0.18f, 0.08f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(screenX - 8.0f * scale, bottom);
            glVertex2f(screenX + 8.0f * scale, bottom);
            glVertex2f(screenX + 8.0f * scale, bottom - baseH * 0.5f);
            glVertex2f(screenX - 8.0f * scale, bottom - baseH * 0.5f);
        glEnd();

        glColor4f(0.18f, 0.55f, 0.22f, 1.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(screenX, top);
            glVertex2f(left, bottom - baseH * 0.3f);
            glVertex2f(right, bottom - baseH * 0.3f);
        glEnd();
    }
}

void Renderer::renderWorld(const WorldStaticAssets& assets, const Camera& camera)
{
    for (std::size_t i = 0; i < assets.count; ++i)
    {
        drawSprite(assets.textureID[i], assets.position[i].x, assets.position[i].y, assets.scale[i], camera);
    }
}

void Renderer::renderPlayer(const MainPlayer& player, const Camera& camera)
{
    if (!player.isAlive) return;

    drawSprite(player.textureID, player.position.x, player.position.y, player.scale, camera);

    float screenX = (player.position.x - camera.position.x) * camera.zoom + (camera.width * 0.5f);
    float screenY = (player.position.y - camera.position.y) * camera.zoom + (camera.height * 0.5f);

    ImGui::GetForegroundDrawList()->AddCircleFilled(
        ImVec2(screenX, screenY),
        12.0f * camera.zoom,
        IM_COL32(0, 255, 100, 255)
    );
}

// Renders complete scene (World + Player)
void Renderer::renderScene(const WorldStaticAssets& assets, const MainPlayer& player, const Camera& camera)
{
    renderWorld(assets, camera);
    renderPlayer(player, camera);
}

void Renderer::drawLoginWindow(UIManager& ui)
{
    ImGui::SetNextWindowPos(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::InputText("Username", ui.username, sizeof(ui.username));
    ImGui::InputText("Password", ui.password, sizeof(ui.password), ImGuiInputTextFlags_Password);

    if (ImGui::Button("Connect"))
    {
        ui.connectPressed = true;
    }

    ImGui::End();
}

void Renderer::drawHUD(const MainPlayer& player, const Camera& camera, const WorldStaticAssets& assets)
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);

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
        ImGui::Text("X: %.2f", player.position.x);
        ImGui::Text("Y: %.2f", player.position.y);

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "CAMERA");
        ImGui::Separator();
        ImGui::Text("Cam X: %.2f", camera.position.x);
        ImGui::Text("Cam Y: %.2f", camera.position.y);

        ImGui::Spacing();
        ImGui::Text("World Static Assets: %zu", assets.count);
    }
    ImGui::End();
}

void Renderer::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow((SDL_Window*)window);
}

void Renderer::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (gl_context) SDL_GL_DestroyContext((SDL_GLContext)gl_context);
    if (window) SDL_DestroyWindow((SDL_Window*)window);
    SDL_Quit();
}