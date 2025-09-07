#pragma once

#include "Node.h"

namespace gui::editor
{
    enum class FixConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
        LoggingIn,
        LoggedIn,
        Error
    };

    struct FixConnectionConfiguration
    {
        std::string senderCompID;
        std::string targetCompID;
        std::string fixVersion;     // "FIX.4.2", "FIX.4.4", "FIX.5.0"
        int heartBeatInt;           // Heartbeat interval in seconds
        bool resetSeqNumFlag;
        std::string logonMessage;
        
        // Connection settings
        int reconnectInterval;      // Seconds between reconnection attempts
        int maxReconnectAttempts;
        bool autoLogon;
        
        FixConnectionConfiguration()
            : fixVersion("FIX.4.4"), heartBeatInt(30), resetSeqNumFlag(false)
            , reconnectInterval(5), maxReconnectAttempts(10), autoLogon(true) {}
    };

    class FixConnectionNode : public Node
    {
    public:
        FixConnectionNode(ax::NodeEditor::NodeId nodeId);
        virtual ~FixConnectionNode() = default;

        // Node interface
        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Connection control
        void StartConnection();
        void StopConnection();
        void ToggleConnection();
        
        // State management
        FixConnectionState GetState() const { return m_state; }
        const std::string& GetStateText() const;
        bool IsConnected() const { return m_state == FixConnectionState::LoggedIn; }

        // Configuration
        const FixConnectionConfiguration& GetConfiguration() const { return m_config; }
        void SetConfiguration(const FixConnectionConfiguration& config) { m_config = config; }

        // Message handling
        void SendMessage(const std::string& message);
        std::vector<std::string> GetRecentMessages(int count = 10);

        // Serialization
        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderConnectionUI();
        void RenderConfigurationUI();
        void RenderControlButtons();
        void RenderStatusDisplay();
        void RenderMessageLog();
        
        void UpdateConnectionState();
        void ProcessMessages();
        void HandleConnectionError(const std::string& error);
        
        ImVec4 GetStateColor() const;
        const char* GetStateIcon() const;

        FixConnectionConfiguration m_config;
        FixConnectionState m_state;
        
        // UI state
        bool m_configExpanded;
        bool m_logExpanded;
        char m_senderCompIDBuffer[64];
        char m_targetCompIDBuffer[64];
        
        // Connection state
        std::string m_lastError;
        float m_connectionStartTime;
        float m_lastHeartbeat;
        int m_reconnectAttempts;
        
        // Message history
        struct FixMessage
        {
            std::string timestamp;
            std::string direction; // "IN" or "OUT"
            std::string message;
            bool isHeartbeat;
        };
        
        std::vector<FixMessage> m_messageHistory;
        static constexpr int MAX_MESSAGE_HISTORY = 100;
        
        // Input validation
        bool HasValidEndpoint() const;
        bool HasValidAccount() const;
        void ValidateInputs();
    };
}