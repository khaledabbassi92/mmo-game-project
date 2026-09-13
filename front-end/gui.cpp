// ============================================================================
// gui.cpp - Entire GUI, Windowing, SDL3 & Dear ImGui Logic
// ============================================================================
#include "core.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>

namespace {
    SDL_Window*   s_window   = nullptr;
    SDL_GLContext s_glContext = nullptr;
    bool          s_running  = false;
    bool          s_connectPressed = false;
    Uint64        s_lastTime = 0;
    float         s_deltaTime = 0.0f;
    const bool*   s_keyboardState = nullptr;
}

bool GUI_Init(int width, int height, const char* title)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return false;
    }

    // Set OpenGL 3.0 Core Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL3 Window with OpenGL Flag
    s_window = SDL_CreateWindow(
        title,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!s_window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create OpenGL Context
    s_glContext = SDL_GL_CreateContext(s_window);
    if (!s_glContext) {
        SDL_Log("SDL_GL_CreateContext Error: %s", SDL_GetError());
        SDL_DestroyWindow(s_window);
        SDL_Quit();
        return false;
    }
    SDL_GL_MakeCurrent(s_window, s_glContext);
    SDL_GL_SetSwapInterval(1); // Enable V-Sync

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends with relative backends/ prefix
    ImGui_ImplSDL3_InitForOpenGL(s_window, s_glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    s_lastTime = SDL_GetPerformanceCounter();
    s_running = true;
    return true;
}

bool GUI_IsRunning()
{
    return s_running;
}

void GUI_BeginFrame()
{
    // Compute Delta Time
    Uint64 currentTime = SDL_GetPerformanceCounter();
    s_deltaTime = static_cast<float>(currentTime - s_lastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
    s_lastTime = currentTime;
    if (s_deltaTime > 0.1f) s_deltaTime = 0.1f;

    s_connectPressed = false;
    ImGuiIO& io = ImGui::GetIO();

    // Event Polling (SDL3 API)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            s_running = false;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(s_window)) {
            s_running = false;
        }
    }

    // Read keyboard state if not typing inside an ImGui input widget
    if (!io.WantCaptureKeyboard) {
        s_keyboardState = SDL_GetKeyboardState(nullptr);
    } else {
        s_keyboardState = nullptr;
    }

    // Clear Screen (Void dark canvas)
    int w, h;
    SDL_GetWindowSize(s_window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.04f, 0.06f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Render Connection / Gateway Control Panel in ImGui
    ImGui::Begin("MMO Client Gateway");
    if (!World_IsSpawned()) {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "Status: DISCONNECTED");
        ImGui::Text("Click 'Connect' to spawn the 2000x2000 World Space.");
        ImGui::Spacing();
        if (ImGui::Button(" CONNECT & SPAWN WORLD ", ImVec2(240, 40))) {
            s_connectPressed = true;
        }
    } else {
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.4f, 1.0f), "Status: IN WORLD (Spawned)");
        if (ImGui::Button(" DESPAWN / DISCONNECT ", ImVec2(200, 28))) {
            World_Despawn();
        }
    }
    ImGui::End();
}

void GUI_EndFrame()
{
    // Render ImGui draw data on top of world background
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap OpenGL Buffers (SDL3)
    SDL_GL_SwapWindow(s_window);
}

void GUI_Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (s_glContext) SDL_GL_DestroyContext(s_glContext);
    if (s_window)    SDL_DestroyWindow(s_window);
    SDL_Quit();

    s_glContext = nullptr;
    s_window = nullptr;
    s_running = false;
}

float GUI_GetDeltaTime()
{
    return s_deltaTime;
}

void GUI_GetWindowSize(int* w, int* h)
{
    if (s_window) {
        SDL_GetWindowSize(s_window, w, h);
    } else {
        if (w) *w = 1280;
        if (h) *h = 720;
    }
}

const bool* GUI_GetKeyboardState()
{
    return s_keyboardState;
}

bool GUI_IsConnectPressed()
{
    return s_connectPressed;
}