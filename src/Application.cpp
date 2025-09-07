#include "Application.h"

#include "explorer/ExplorerDialog.h"

#include "window/StartupWindow.h"
#include "window/ConnectionPanel.h"
#include "window/ExchangeControlPanel.h"
#include "window/ExchangeEditor.h"

#include <stdio.h>

namespace gui {
Application::Application()
    : m_currentState(State::Startup)
    , m_shouldClose(false)
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    // Setup ImGui configuration with docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Create window components
    m_startupWindow = std::make_unique<gui::window::StartupWindow>();
    m_connectionPanel = std::make_unique<gui::window::ConnectionPanel>();
    m_exchangeControlPanel = std::make_unique<gui::window::ExchangeControlPanel>();
    m_exchangeEditor = std::make_unique<gui::window::ExchangeEditor>();
    
    // Initialize all components
    if (!m_startupWindow->Initialize())
    {
        printf("Failed to initialize StartupWindow\n");
        return false;
    }
    
    if (!m_connectionPanel->Initialize())
    {
        printf("Failed to initialize ConnectionPanel\n");
        return false;
    }
    
    if (!m_exchangeControlPanel->Initialize())
    {
        printf("Failed to initialize ExchangeControlPanel\n");
        return false;
    }
    
    if (!m_exchangeEditor->Initialize())
    {
        printf("Failed to initialize ExchangeEditor\n");
        return false;
    }
    
    printf("Application initialized successfully\n");
    return true;
}

void Application::Render()
{
    UpdateState();
    RenderCurrentWindow();
}

void Application::UpdateState()
{
    switch (m_currentState)
    {
        case State::Startup:
        {
            auto action = m_startupWindow->GetSelectedAction();
            switch (action)
            {
                case gui::window::StartupWindow::Action::CreateNewExchange:
                    m_startupWindow->ResetAction();
                    m_startupWindow->Hide();
                    m_exchangeEditor->Show();
                    m_currentState = State::ExchangeEditor;
                    printf("Transitioning to ExchangeEditor\n");
                    break;
                    
                case gui::window::StartupWindow::Action::LoadExchange:
                    // Note: Loading is handled within StartupWindow via ExplorerDialog
                    m_startupWindow->ResetAction();
                    break;
                    
                case gui::window::StartupWindow::Action::OpenConnection:
                    m_startupWindow->ResetAction();
                    m_startupWindow->Hide();
                    m_connectionPanel->Show();
                    m_currentState = State::Connection;
                    printf("Transitioning to ConnectionPanel\n");
                    break;
                    
                case gui::window::StartupWindow::Action::Close:
                    m_shouldClose = true;
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case State::Connection:
        {
            auto action = m_connectionPanel->GetAction();
            switch (action)
            {
                case gui::window::ConnectionPanel::Action::Back:
                    m_connectionPanel->ResetAction();
                    m_connectionPanel->Hide();
                    m_startupWindow->Show();
                    m_currentState = State::Startup;
                    printf("Returning to StartupWindow\n");
                    break;
                    
                case gui::window::ConnectionPanel::Action::Connect:
                {
                    auto config = m_connectionPanel->GetConnectionConfig();
                    m_selectedExchange = config.exchangeName;
                    m_selectedCurrencyPair = config.currencyPair;
                    
                    m_connectionPanel->ResetAction();
                    m_connectionPanel->Hide();
                    
                    m_exchangeControlPanel->SetExchangeInfo(m_selectedExchange, m_selectedCurrencyPair);
                    m_exchangeControlPanel->Show();
                    m_currentState = State::ExchangeControl;
                    printf("Connected to %s (%s), transitioning to ExchangeControlPanel\n", 
                           m_selectedExchange.c_str(), m_selectedCurrencyPair.c_str());
                    break;
                }
                
                case gui::window::ConnectionPanel::Action::Close:
                    m_shouldClose = true;
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case State::ExchangeControl:
        {
            auto action = m_exchangeControlPanel->GetAction();
            switch (action)
            {
                case gui::window::ExchangeControlPanel::Action::Back:
                    m_exchangeControlPanel->ResetAction();
                    m_exchangeControlPanel->Hide();
                    m_startupWindow->Show();
                    m_currentState = State::Startup;
                    printf("Returning to StartupWindow\n");
                    break;
                    
                case gui::window::ExchangeControlPanel::Action::Close:
                    m_shouldClose = true;
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case State::ExchangeEditor:
        {
            auto action = m_exchangeEditor->GetAction();
            switch (action)
            {
                case gui::window::ExchangeEditor::Action::Back:
                    m_exchangeEditor->ResetAction();
                    m_exchangeEditor->Hide();
                    m_startupWindow->Show();
                    m_currentState = State::Startup;
                    printf("Returning to StartupWindow\n");
                    break;
                    
                case gui::window::ExchangeEditor::Action::Save:
                    m_exchangeEditor->ResetAction();
                    printf("Exchange configuration saved\n");
                    break;
                    
                case gui::window::ExchangeEditor::Action::Close:
                    m_shouldClose = true;
                    break;
                    
                default:
                    break;
            }
            break;
        }
        
        case State::Shutdown:
            m_shouldClose = true;
            break;
    }
}

void Application::RenderCurrentWindow()
{
    switch (m_currentState)
    {
        case State::Startup:
            if (m_startupWindow->ShouldShow())
                m_startupWindow->Render();
            break;
            
        case State::Connection:
            if (m_connectionPanel->ShouldShow())
                m_connectionPanel->Render();
            break;
            
        case State::ExchangeControl:
            if (m_exchangeControlPanel->ShouldShow())
                m_exchangeControlPanel->Render();
            break;
            
        case State::ExchangeEditor:
            if (m_exchangeEditor->ShouldShow())
                m_exchangeEditor->Render();
            break;
            
        case State::Shutdown:
        default:
            break;
    }
}

void Application::Shutdown()
{
    if (m_startupWindow)
        m_startupWindow->Shutdown();
    if (m_connectionPanel)
        m_connectionPanel->Shutdown();
    if (m_exchangeControlPanel)
        m_exchangeControlPanel->Shutdown();
    if (m_exchangeEditor)
        m_exchangeEditor->Shutdown();
        
    printf("Application shutdown complete\n");
}

bool Application::ShouldClose() const
{
    return m_shouldClose;
}
}