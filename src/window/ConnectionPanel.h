#pragma once

#include "imgui.h"
#include <string>
#include <vector>

namespace gui {
namespace window {

class ConnectionPanel
{
public:
    enum class Action
    {
        None,
        Back,
        Connect,
        Close
    };

    struct ConnectionConfig
    {
        std::string exchangeName;
        std::string currencyPair;
        std::string certificateFile;
        std::string keyFile;
        std::string username;
        std::string password;
        std::string serverUrl;
        int port;
        bool useSSL;
    };

    ConnectionPanel();
    ~ConnectionPanel();

    bool Initialize();
    void Render();
    void Shutdown();

    Action GetAction() const { return m_action; }
    void ResetAction() { m_action = Action::None; }

    const ConnectionConfig& GetConnectionConfig() const { return m_config; }

    bool ShouldShow() const { return m_shouldShow; }
    void Show() { m_shouldShow = true; }
    void Hide() { m_shouldShow = false; }

private:
    void RenderExchangeSelection();
    void RenderCurrencyPairSelection();
    void RenderConnectionParameters();
    void RenderButtons();

    bool m_shouldShow;
    Action m_action;
    ConnectionConfig m_config;

    // UI state
    size_t m_selectedExchangeIndex;
    size_t m_selectedPairIndex;

    // Data
    std::vector<std::string> m_availableExchanges;
    std::vector<std::string> m_availablePairs;
};
} // namespace window
} // namespace gui
