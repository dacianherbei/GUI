#pragma once

#include "imgui.h"
#include <string>
#include <vector>

namespace gui
{
    // Forward declarations for your exchange types
    struct ExchangeConfig;
    struct ExchangeConnection;

    class MainWindow
    {
    public:
        MainWindow();
        ~MainWindow();

        bool Initialize();
        void Render();
        void Shutdown();
        bool ShouldClose() const;

    private:
        // Menu callbacks - implement your exchange logic here
        void HandleCreateNewExchange();
        void HandleLoadExchange();
        void HandleConnectToExchange();

        // Additional UI rendering methods
        void RenderMenuBar();
        void RenderStatusBar();
        void RenderExchangeList();
        void RenderConnectionStatus();
        void RenderOrderBook();
        void RenderTradingPanel();

        // Window state
        bool m_shouldClose;
        bool m_showCreateNewDialog;
        bool m_showLoadDialog;
        bool m_showConnectDialog;
        bool m_showAboutDialog;

        // Exchange-specific state
        struct ExchangeState
        {
            bool isConnected;
            std::string exchangeName;
            std::string connectionStatus;
            std::vector<std::string> availableSymbols;
            // Add your exchange data structures here
        };

        ExchangeState m_exchangeState;

        // UI state
        struct UIState
        {
            bool showOrderBook;
            bool showTradingPanel;
            bool showLogs;
            int selectedSymbolIndex;
            char symbolFilter[128];
        };

        UIState m_uiState;

        // Configuration
        static constexpr int DEFAULT_WINDOW_WIDTH = 1200;
        static constexpr int DEFAULT_WINDOW_HEIGHT = 800;
    };
}