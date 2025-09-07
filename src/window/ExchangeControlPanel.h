#pragma once

#include "imgui.h"

#include <memory>
#include <string>

namespace gui::panel {
    class LoggingPanel;
    class OrderBookPanel;
    class TradingPanel;
}

namespace gui {
namespace window {

class ExchangeControlPanel
{
public:
    enum class Action
    {
        None,
        Back,
        Close
    };

    ExchangeControlPanel();
    ~ExchangeControlPanel();

    bool Initialize();
    void Render();
    void Shutdown();

    Action GetAction() const { return m_action; }
    void ResetAction() { m_action = Action::None; }

    bool ShouldShow() const { return m_shouldShow; }
    void Show() { m_shouldShow = true; }
    void Hide() { m_shouldShow = false; }

    // Set exchange information
    void SetExchangeInfo(const std::string& exchangeName, const std::string& currencyPair);

private:
    void RenderDockSpace();
    void RenderMenuBar();
    void RenderStatusBar();

    bool m_shouldShow;
    Action m_action;
    bool m_dockspaceInitialized;

    // Exchange information
    std::string m_exchangeName;
    std::string m_currencyPair;

    // Component panels
    std::unique_ptr<gui::panel::LoggingPanel> m_loggingPanel;
    std::unique_ptr<gui::panel::OrderBookPanel> m_orderBookPanel;
    std::unique_ptr<gui::panel::TradingPanel> m_tradingPanel;

    // Window visibility
    bool m_showLoggingPanel;
    bool m_showOrderBookPanel;
    bool m_showTradingPanel;
};

} // namespace window
} // namespace gui
