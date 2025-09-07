#include "ExchangeControlPanel.h"

#include "panel/LoggingPanel.h"
#include "panel/OrderBookPanel.h"
#include "panel/TradingPanel.h"

#include "imgui_internal.h"

#include <stdio.h>

namespace gui {
namespace window {

ExchangeControlPanel::ExchangeControlPanel()
    : m_shouldShow(false)
    , m_action(Action::None)
    , m_dockspaceInitialized(false)
    , m_exchangeName("Unknown")
    , m_currencyPair("BTC/USD")
    , m_showLoggingPanel(true)
    , m_showOrderBookPanel(true)
    , m_showTradingPanel(true)
{
}

ExchangeControlPanel::~ExchangeControlPanel()
{
    Shutdown();
}

bool ExchangeControlPanel::Initialize()
{
    // Create component panels
    m_loggingPanel = std::make_unique<gui::panel::LoggingPanel>();
    m_orderBookPanel = std::make_unique<gui::panel::OrderBookPanel>();
    m_tradingPanel = std::make_unique<gui::panel::TradingPanel>();

    // Initialize all panels
    if (!m_loggingPanel->Initialize())
    {
        printf("Failed to initialize LoggingPanel\n");
        return false;
    }

    if (!m_orderBookPanel->Initialize())
    {
        printf("Failed to initialize OrderBookPanel\n");
        return false;
    }

    if (!m_tradingPanel->Initialize())
    {
        printf("Failed to initialize TradingPanel\n");
        return false;
    }

    // Set up trading panel callback
    m_tradingPanel->SetOrderCallback([this](const gui::panel::TradingPanel::OrderRequest& order) {
        // Log the order
        char logMsg[256];
        snprintf(logMsg, sizeof(logMsg), "Order placed: %s %s %.4f %s at %.2f",
                (order.side == gui::panel::TradingPanel::OrderSide::Buy) ? "BUY" : "SELL",
                (order.type == gui::panel::TradingPanel::OrderType::Market) ? "Market" :
                (order.type == gui::panel::TradingPanel::OrderType::Limit) ? "Limit" : "Stop",
                order.amount, order.currencyPair.c_str(), order.price);

        m_loggingPanel->AddLog(logMsg, gui::panel::LoggingPanel::LogLevel::Success);
    });

    return true;
}

void ExchangeControlPanel::Render()
{
    if (!m_shouldShow)
        return;

    // Create main dockspace window
    RenderDockSpace();

    // Render component panels
    if (m_showLoggingPanel && m_loggingPanel)
        m_loggingPanel->Render(&m_showLoggingPanel);

    if (m_showOrderBookPanel && m_orderBookPanel)
        m_orderBookPanel->Render(&m_showOrderBookPanel);

    if (m_showTradingPanel && m_tradingPanel)
        m_tradingPanel->Render(&m_showTradingPanel);

    // Render status bar
    RenderStatusBar();
}

void ExchangeControlPanel::RenderDockSpace()
{
    // Create main dockspace window that fills the viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Make the dockspace window transparent and always in background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    std::string windowTitle = "Exchange Control - " + m_exchangeName + " (" + m_currencyPair + ")";
    ImGui::Begin(windowTitle.c_str(), nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Render menu bar
    RenderMenuBar();

    // Create dockspace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("ExchangeControlDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        // Initialize default docking layout (only once)
        if (!m_dockspaceInitialized)
        {
            m_dockspaceInitialized = true;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

            // Split the dockspace into regions
            // Left side: Trading Panel (30%)
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);
            // Bottom: Log Window (25%)
            auto dock_id_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.33f, nullptr, &dockspace_id);
            // Center: Order Book (remaining space)

            // Dock windows to specific areas
            ImGui::DockBuilderDockWindow("Trading Panel", dock_id_left);
            ImGui::DockBuilderDockWindow("Order Book", dockspace_id);
            ImGui::DockBuilderDockWindow("Log Window", dock_id_bottom);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    ImGui::End();
}

void ExchangeControlPanel::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Exchange"))
        {
            if (ImGui::MenuItem("Disconnect", "Ctrl+D"))
            {
                m_action = Action::Back;
                m_loggingPanel->AddLog("Disconnecting from exchange...", gui::panel::LoggingPanel::LogLevel::Warning);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
            {
                m_action = Action::Close;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Trading Panel", nullptr, &m_showTradingPanel);
            ImGui::MenuItem("Order Book", nullptr, &m_showOrderBookPanel);
            ImGui::MenuItem("Log Window", nullptr, &m_showLoggingPanel);

            ImGui::Separator();

            if (ImGui::MenuItem("Reset Layout"))
            {
                m_dockspaceInitialized = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Trading"))
        {
            if (ImGui::MenuItem("Cancel All Orders"))
            {
                m_loggingPanel->AddLog("Cancel all orders requested", gui::panel::LoggingPanel::LogLevel::Warning);
            }

            if (ImGui::MenuItem("Refresh Market Data"))
            {
                m_loggingPanel->AddLog("Market data refreshed", gui::panel::LoggingPanel::LogLevel::Info);
                // TODO: Actually refresh market data
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void ExchangeControlPanel::RenderStatusBar()
{
    // Create a status bar using a viewport-sized window at the bottom
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - ImGui::GetFrameHeight()));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, ImGui::GetFrameHeight()));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("##ExchangeStatusBar", nullptr, flags))
    {
        if (ImGui::BeginMenuBar())
        {
            // Connection status
            ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "● Connected to %s", m_exchangeName.c_str());
            ImGui::Separator();

            // Current pair and price
            ImGui::Text("%s: $50,123.45 (+0.52%%)", m_currencyPair.c_str());
            ImGui::Separator();

            // Trading info
            ImGui::Text("Orders: 0 Active");
            ImGui::Separator();

            // Network latency
            ImGui::Text("Latency: 15ms");
            ImGui::Separator();

            // System time
            ImGui::Text("Time: %.1fs", ImGui::GetTime());

            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
}

void ExchangeControlPanel::SetExchangeInfo(const std::string& exchangeName, const std::string& currencyPair)
{
    m_exchangeName = exchangeName;
    m_currencyPair = currencyPair;

    // Update component panels with new information
    if (m_orderBookPanel)
    {
        m_orderBookPanel->SetCurrencyPair(currencyPair);
    }

    if (m_tradingPanel)
    {
        m_tradingPanel->SetCurrencyPair(currencyPair);
    }

    if (m_loggingPanel)
    {
        char logMsg[256];
        snprintf(logMsg, sizeof(logMsg), "Connected to %s for %s trading",
                exchangeName.c_str(), currencyPair.c_str());
        m_loggingPanel->AddLog(logMsg, gui::panel::LoggingPanel::LogLevel::Success);
    }

    // Reset dockspace to ensure proper layout with new title
    m_dockspaceInitialized = false;
}

void ExchangeControlPanel::Shutdown()
{
    if (m_loggingPanel)
        m_loggingPanel->Shutdown();
    if (m_orderBookPanel)
        m_orderBookPanel->Shutdown();
    if (m_tradingPanel)
        m_tradingPanel->Shutdown();
}
} // namespace window
} // namespace gui