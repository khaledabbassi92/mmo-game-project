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

static SDL_Window* window = nullptr;
static SDL_GLContext gl_context = nullptr;
static Uint64 last_counter = 0;
static float delta_time = 0.0f;
static bool app_running = true;

bool Renderer_Init(int width, int height, const char* title)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Use Compatibility profile so 2D immediate mode and ImGui run seamlessly
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        std::cerr << "GL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplSDL3_InitForOpenGL(window, gl_context) || !ImGui_ImplOpenGL3_Init("#version 120"))
    {
        std::cerr << "ImGui initialization failed." << std::endl;
        return false;
    }

    camera.width = static_cast<float>(width);
    camera.height = static_cast<float>(height);
    last_counter = SDL_GetTicksNS();
    return true;
}

bool Renderer_IsRunning()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT || 
           (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)))
        {
            app_running = false;
        }
    }
    return app_running;
}

void Renderer_BeginFrame()
{
    Uint64 current_counter = SDL_GetTicksNS();
    delta_time = (float)((double)(current_counter - last_counter) / 1000000000.0);
    last_counter = current_counter;

    int drawableW = 0, drawableH = 0;
    SDL_GetWindowSizeInPixels(window, &drawableW, &drawableH);
    camera.width = (float)drawableW;
    camera.height = (float)drawableH;

    glViewport(0, 0, drawableW, drawableH);
    glClearColor(28.0f / 255.0f, 42.0f / 255.0f, 26.0f / 255.0f, 1.0f); // Forest meadow dark green
    glClear(GL_COLOR_BUFFER_BIT);

    // Set 2D Orthographic Projection in screen pixel space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, drawableW, drawableH, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

// Corrected Sprite Rendering: Applies Camera Transformation, Scaling & Anchor Point
void Render_DrawSprite(unsigned int textureID, float worldX, float worldY, float scale)
{
    // 1. World space to Screen space projection
    float screenX = (worldX - camera.position.x) * camera.zoom + (camera.width * 0.5f);
    float screenY = (worldY - camera.position.y) * camera.zoom + (camera.height * 0.5f);

    float baseW = 120.0f * scale * camera.zoom;
    float baseH = 160.0f * scale * camera.zoom;

    // Anchor at trunk base (bottom-center)
    float left   = screenX - (baseW * 0.5f);
    float right  = screenX + (baseW * 0.5f);
    float bottom = screenY;
    float top    = screenY - baseH;

    // Frustum Culling
    if (right < 0 || left > camera.width || bottom < 0 || top > camera.height)
        return;

    // 2. Drop Shadow under the tree
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(screenX, screenY);
    for (int i = 0; i <= 16; ++i) {
        float angle = i * (2.0f * 3.14159265f / 16.0f);
        glVertex2f(screenX + std::cos(angle) * (baseW * 0.35f), 
                   screenY + std::sin(angle) * (baseW * 0.15f));
    }
    glEnd();

    // 3. Render Texture Quad if loaded
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
        // Procedural Fallback Quad so tree is NEVER invisible
        glColor4f(0.3f, 0.18f, 0.08f, 1.0f); // Trunk
        glBegin(GL_QUADS);
            glVertex2f(screenX - 8.0f * scale, bottom);
            glVertex2f(screenX + 8.0f * scale, bottom);
            glVertex2f(screenX + 8.0f * scale, bottom - baseH * 0.5f);
            glVertex2f(screenX - 8.0f * scale, bottom - baseH * 0.5f);
        glEnd();

        glColor4f(0.18f, 0.55f, 0.22f, 1.0f); // Foliage
        glBegin(GL_TRIANGLES);
            glVertex2f(screenX, top);
            glVertex2f(left, bottom - baseH * 0.3f);
            glVertex2f(right, bottom - baseH * 0.3f);
        glEnd();
    }
}

void Renderer_EndFrame()
{
    switch (current_state)
    {
        case GameState::Login:
            DrawLoginWindow();
            break;
        case GameState::Playing:
            world.renderWorld();
            DrawHUD();
            break;
        case GameState::Options:
            break;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void Renderer_Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

float Renderer_GetDeltaTime()
{
    return delta_time;
}