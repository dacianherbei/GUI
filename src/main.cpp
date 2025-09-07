// main.cpp
// Entry point for the Exchange GUI application using SDL2 + OpenGL3
// main.cpp
// Entry point for the Exchange GUI application using SDL2 + OpenGL2

#include "MainWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>

// Global variables for SDL2 + OpenGL2 backend
static SDL_Window* g_window = nullptr;
static SDL_GLContext g_gl_context = nullptr;

int main(int argc, char** argv)
{
    printf("Starting Exchange GUI Application (OpenGL2)\n");

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup OpenGL2 context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    g_window = SDL_CreateWindow(
        "Exchange GUI Application",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1200,
        800,
        window_flags
    );

    if (g_window == nullptr)
    {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    g_gl_context = SDL_GL_CreateContext(g_window);
    if (g_gl_context == nullptr)
    {
        fprintf(stderr, "SDL_GL_CreateContext Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_MakeCurrent(g_window, g_gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Print OpenGL version info
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Initialize ImGui backends for SDL2 + OpenGL2
    ImGui_ImplSDL2_InitForOpenGL(g_window, g_gl_context);
    ImGui_ImplOpenGL2_Init();

    // Create main window
    gui::MainWindow mainWindow;
    if (!mainWindow.Initialize())
    {
        fprintf(stderr, "Failed to initialize main window\n");
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_GL_DeleteContext(g_gl_context);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return -1;
    }

    printf("Application initialized successfully\n");

    // Main application loop
    bool done = false;
    while (!done && !mainWindow.ShouldClose())
    {
        // Poll and handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(g_window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render main window
        mainWindow.Render();

        // Rendering
        ImGui::Render();

        // OpenGL2 rendering
        int display_w, display_h;
        SDL_GetWindowSize(g_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(g_window);
    }

    printf("Shutting down application\n");

    // Cleanup
    mainWindow.Shutdown();

    // Shutdown ImGui backends
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    ImGui::DestroyContext();

    if (g_gl_context)
    {
        SDL_GL_DeleteContext(g_gl_context);
        g_gl_context = nullptr;
    }

    if (g_window)
    {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    SDL_Quit();

    printf("Application closed successfully\n");
    return 0;
}