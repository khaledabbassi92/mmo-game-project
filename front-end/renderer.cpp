#include "core.h"
#include <iostream>
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
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window)
    {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        std::cout << "GL context creation failed: " << SDL_GetError() << std::endl;
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

    if (!ImGui_ImplSDL3_InitForOpenGL(window, gl_context) || !ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cout << "ImGui initialization failed." << std::endl;
        return false;
    }

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

    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0)
    {
        SDL_Delay(10);
    }

    return app_running;
}

void Renderer_BeginFrame()
{
    Uint64 current_counter = SDL_GetTicksNS();
    delta_time = (float)((double)(current_counter - last_counter) / 1000000000.0);
    last_counter = current_counter;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer_EndFrame()
{
    GameState current_state = Core_GetState();

    switch (current_state)
    {
        case GameState::Login:
            DrawLoginWindow();
            break;
        case GameState::Playing:
            Core_GetWorld().renderWorld();
            break;
        case GameState::Options:
            break;
    }

    ImGui::Render();

    int drawableW = 0, drawableH = 0;
    SDL_GetWindowSizeInPixels(window, &drawableW, &drawableH);
    glViewport(0, 0, drawableW, drawableH);
    glClearColor(25.0f / 255.0f, 25.0f / 255.0f, 30.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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