#pragma once

#include "Node.h"
#include <queue>

namespace gui::editor
{
    enum class WebSocketState
    {
        Disconnected,
        Connecting,
        Connected,
        Authenticating,
        Authenticated,
        Error
    };

    struct WebSocketConfiguration
    {
        std::string subProtocol;        // WebSocket sub-protocol
        std::vector<std::string> headers; // Custom headers
        bool enablePing;                // Send ping frames
        int pingInterval;               // Seconds between pings
        int connectionTimeout;          // Connection timeout in seconds
        bool autoReconnect;
        int maxReconnectAttempts;
        
        // Authentication
        std::string authMethod;         // "None", "Bearer", "ApiKey", "Custom"
        std::string authToken;
        std::string authHeader;
        
        WebSocketConfiguration()
            : enablePing(true), pingInterval(30), connectionTimeout(10)
            , autoReconnect(true), maxReconnectAttempts(5), authMethod("None") {}
    };

    class WebSocketConnectionNode : public Node
    {
    public:
        WebSocketConnectionNode(ax::NodeEditor::NodeId nodeId);
        virtual ~WebSocketConnectionNode() = default;

        // Node interface
        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Connection control
        void Connect();
        void Disconnect();
        void ToggleConnection();
        
        // State management
        WebSocketState GetState() const { return m_state; }
        const std::string& GetStateText() const;
        bool IsConnected() const { return m_state == WebSocketState::Authenticated || 
                                         m_state == WebSocketState::Connected; }

        // Message handling
        void SendMessage(const std::string& message);
        void SendJson(const std::string& jsonMessage);
        void Subscribe(const std::string& channel);
        void Unsubscribe(const std::string& channel);

        // Configuration
        const WebSocketConfiguration& GetConfiguration() const { return m_config; }
        void SetConfiguration(const WebSocketConfiguration& config) { m_config = config; }

        // Subscriptions
        const std::vector<std::string>& GetActiveSubscriptions() const { return m_activeSubscriptions; }
        void ClearSubscriptions();

        // Serialization
        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderConnectionUI();
        void RenderConfigurationUI();
        void RenderControlButtons();
        void RenderStatusDisplay();
        void RenderSubscriptionManager();
        void RenderMessageLog();
        
        void UpdateConnectionState();
        void ProcessIncomingMessages();
        void HandleConnectionError(const std::string& error);
        void SendPing();
        
        ImVec4 GetStateColor() const;
        const char* GetStateIcon() const;

        WebSocketConfiguration m_config;
        WebSocketState m_state;
        
        // UI state
        bool m_configExpanded;
        bool m_subscriptionsExpanded;
        bool m_logExpanded;
        char m_customHeaderBuffer[256];
        char m_subscriptionBuffer[128];
        
        // Connection state
        std::string m_lastError;
        float m_connectionStartTime;
        float m_lastPing;
        int m_reconnectAttempts;
        
        // Subscriptions
        std::vector<std::string> m_activeSubscriptions;
        std::queue<std::string> m_pendingSubscriptions;
        std::queue<std::string> m_pendingUnsubscriptions;
        
        // Message handling
        struct WebSocketMessage
        {
            std::string timestamp;
            std::string direction; // "IN", "OUT", "PING", "PONG"
            std::string message;
            std::string messageType; // "Text", "Binary", "Control"
            size_t size;
        };
        
        std::vector<WebSocketMessage> m_messageHistory;
        static constexpr int MAX_MESSAGE_HISTORY = 100;
        
        // Statistics
        struct Statistics
        {
            int messagesSent;
            int messagesReceived;
            int bytesReceived;
            int bytesSent;
            int reconnectCount;
            float uptime;
            
            Statistics() : messagesSent(0), messagesReceived(0), 
                          bytesReceived(0), bytesSent(0), 
                          reconnectCount(0), uptime(0.0f) {}
        };
        
        Statistics m_stats;
        
        // Input validation
        bool HasValidEndpoint() const;
        bool HasValidAccount() const;
        void ValidateInputs();
        
        // Common WebSocket endpoints for crypto exchanges
        struct ExchangeEndpoint
        {
            std::string name;
            std::string url;
            std::string authMethod;
            std::vector<std::string> commonChannels;
        };
        
        static std::vector<ExchangeEndpoint> GetExchangeEndpoints();
        void ApplyExchangePreset(const ExchangeEndpoint& endpoint);
    };
}