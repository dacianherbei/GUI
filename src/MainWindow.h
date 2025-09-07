#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace gui
{
    class MainWindow
    {
    public:
        MainWindow();
        ~MainWindow();

        // Initialize the main window
        bool Initialize();

        // Render the main window
        void Render();

        // Cleanup resources
        void Shutdown();

        // Check if the window should close
        bool ShouldClose() const;

    private:
        // Menu callbacks
        void HandleCreateNewExchange();
        void HandleLoadExchange();
        void HandleConnectToExchange();

        // Main menu bar rendering
        void RenderMenuBar();

        // Window state
        bool m_shouldClose;
        bool m_showCreateNewDialog;
        bool m_showLoadDialog;
        bool m_showConnectDialog;

        // Window properties
        static constexpr int DEFAULT_WINDOW_WIDTH = 1200;
        static constexpr int DEFAULT_WINDOW_HEIGHT = 800;
    };
}