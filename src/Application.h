#pragma once

#include "imgui.h"

#include <memory>
#include <string>

namespace gui::window {
    class StartupWindow;
    class ConnectionPanel;
    class ExchangeControlPanel;
    class ExchangeEditor;
}

namespace gui {
class Application
{
public:
    enum class State
    {
        Startup,
        Connection,
        ExchangeControl,
        ExchangeEditor,
        Shutdown
    };

    Application();
    ~Application();
    
    bool Initialize();
    void Render();
    void Shutdown();
    
    bool ShouldClose() const;

private:
    void UpdateState();
    void RenderCurrentWindow();
    
    State m_currentState;
    bool m_shouldClose;
    
    // Window components
    std::unique_ptr<gui::window::StartupWindow> m_startupWindow;
    std::unique_ptr<gui::window::ConnectionPanel> m_connectionPanel;
    std::unique_ptr<gui::window::ExchangeControlPanel> m_exchangeControlPanel;
    std::unique_ptr<gui::window::ExchangeEditor> m_exchangeEditor;
    
    // Shared data
    std::string m_selectedExchange;
    std::string m_selectedCurrencyPair;
};
}