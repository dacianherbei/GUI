#include "ConnectionPanel.h"

#include <stdio.h>

namespace gui {
namespace window {

ConnectionPanel::ConnectionPanel()
    : m_shouldShow(false)
    , m_action(Action::None)
    , m_selectedExchangeIndex(0)
    , m_selectedPairIndex(0)
{
    // Initialize default values
    m_config.port = 443;
    m_config.useSSL = true;
}

ConnectionPanel::~ConnectionPanel()
{
    Shutdown();
}

bool ConnectionPanel::Initialize()
{
    // Populate available exchanges
    m_availableExchanges = {
        "Binance",
        "Coinbase Pro",
        "Kraken",
        "BitMEX",
        "FTX",
        "Huobi",
        "OKEx",
        "Bitfinex"
    };

    // Populate available currency pairs
    m_availablePairs = {
        "BTC/USD",
        "ETH/USD",
        "BTC/EUR",
        "ETH/BTC",
        "LTC/USD",
        "XRP/USD",
        "ADA/USD",
        "DOT/USD",
        "LINK/USD",
        "SOL/USD"
    };

    return true;
}

void ConnectionPanel::Render()
{
    if (!m_shouldShow)
        return;

    // Center the connection panel
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(600, 500);
    ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Connect to Exchange", nullptr, flags))
    {
        ImGui::Text("Configure your exchange connection");
        ImGui::Separator();
        ImGui::Spacing();

        RenderExchangeSelection();
        ImGui::Spacing();

        RenderCurrencyPairSelection();
        ImGui::Spacing();

        RenderConnectionParameters();
        ImGui::Spacing();

        ImGui::Separator();
        RenderButtons();
    }
    ImGui::End();
}

void ConnectionPanel::RenderExchangeSelection()
{
    ImGui::Text("Select Exchange:");

    if (ImGui::BeginCombo("##exchange", m_availableExchanges[m_selectedExchangeIndex].c_str()))
    {
        for (size_t i = 0; i < m_availableExchanges.size(); i++)
        {
            bool isSelected = (m_selectedExchangeIndex == i);
            if (ImGui::Selectable(m_availableExchanges[i].c_str(), isSelected))
            {
                m_selectedExchangeIndex = i;
                m_config.exchangeName = m_availableExchanges[i];
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    m_config.exchangeName = m_availableExchanges[m_selectedExchangeIndex];
}

void ConnectionPanel::RenderCurrencyPairSelection()
{
    ImGui::Text("Select Currency Pair:");

    if (ImGui::BeginCombo("##pair", m_availablePairs[m_selectedPairIndex].c_str()))
    {
        for (size_t i = 0; i < m_availablePairs.size(); i++)
        {
            bool isSelected = (m_selectedPairIndex == i);
            if (ImGui::Selectable(m_availablePairs[i].c_str(), isSelected))
            {
                m_selectedPairIndex = i;
                m_config.currencyPair = m_availablePairs[i];
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    m_config.currencyPair = m_availablePairs[m_selectedPairIndex];
}

void ConnectionPanel::RenderConnectionParameters()
{
    ImGui::Text("Connection Parameters:");

    // Server URL
    static char serverUrl[256] = "";
    if (m_config.serverUrl.empty())
    {
        // Set default URLs based on exchange
        if (m_config.exchangeName == "Binance")
            strcpy(serverUrl, "api.binance.com");
        else if (m_config.exchangeName == "Coinbase Pro")
            strcpy(serverUrl, "api.pro.coinbase.com");
        else
            strcpy(serverUrl, "api.exchange.com");
    }
    else
    {
        strcpy(serverUrl, m_config.serverUrl.c_str());
    }

    ImGui::InputText("Server URL", serverUrl, sizeof(serverUrl));
    m_config.serverUrl = serverUrl;

    // Port
    ImGui::InputInt("Port", &m_config.port);

    // SSL checkbox
    ImGui::Checkbox("Use SSL/TLS", &m_config.useSSL);

    ImGui::Separator();

    // Authentication
    ImGui::Text("Authentication:");

    static char username[128] = "";
    static char password[128] = "";
    static char certFile[256] = "";
    static char keyFile[256] = "";

    ImGui::InputText("Username/API Key", username, sizeof(username));
    m_config.username = username;

    ImGui::InputText("Password/Secret", password, sizeof(password), ImGuiInputTextFlags_Password);
    m_config.password = password;

    ImGui::InputText("Certificate File", certFile, sizeof(certFile));
    ImGui::SameLine();
    if (ImGui::Button("Browse##cert"))
    {
        // TODO: Open file dialog
    }
    m_config.certificateFile = certFile;

    ImGui::InputText("Private Key File", keyFile, sizeof(keyFile));
    ImGui::SameLine();
    if (ImGui::Button("Browse##key"))
    {
        // TODO: Open file dialog
    }
    m_config.keyFile = keyFile;
}

void ConnectionPanel::RenderButtons()
{
    float buttonWidth = 100.0f;
    float availWidth = ImGui::GetContentRegionAvail().x;

    // Back button (left side)
    if (ImGui::Button("⬅️ Back", ImVec2(buttonWidth, 0)))
    {
        m_action = Action::Back;
    }

    // Connect button (right side)
    ImGui::SameLine(availWidth - buttonWidth);

    // Enable/disable connect button based on required fields
    bool canConnect = !m_config.exchangeName.empty() &&
                     !m_config.currencyPair.empty() &&
                     !m_config.serverUrl.empty();

    if (!canConnect)
        ImGui::BeginDisabled();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
    if (ImGui::Button("🌐 Connect", ImVec2(buttonWidth, 0)))
    {
        m_action = Action::Connect;
    }
    ImGui::PopStyleColor();

    if (!canConnect)
        ImGui::EndDisabled();

    // Show connection status/info
    ImGui::Spacing();
    ImGui::Text("Connection Info:");
    ImGui::Text("Exchange: %s", m_config.exchangeName.c_str());
    ImGui::Text("Pair: %s", m_config.currencyPair.c_str());
    ImGui::Text("Server: %s:%d (%s)", m_config.serverUrl.c_str(), m_config.port, m_config.useSSL ? "SSL" : "Plain");
}

void ConnectionPanel::Shutdown()
{
    // Cleanup if needed
}
} // namespace windows
} // namespace gui