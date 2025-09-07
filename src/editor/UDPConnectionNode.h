#pragma once

#include "Node.h"

namespace gui::editor
{
    // UDP Connection Node - Similar to WebSocket but for UDP protocol
    enum class UDPConnectionState
    {
        Disconnected,
        Binding,
        Bound,
        Receiving,
        Error
    };

    struct UDPConfiguration
    {
        std::string multicastGroup;
        std::string networkInterface;
        int bufferSize;
        bool enableBroadcast;
        bool reuseAddress;
        int receiveTimeout; // milliseconds

        UDPConfiguration()
            : bufferSize(65536), enableBroadcast(false), reuseAddress(true), receiveTimeout(1000) {}
    };

    class UDPConnectionNode : public Node
    {
    public:
        UDPConnectionNode(ax::NodeEditor::NodeId nodeId);
        virtual ~UDPConnectionNode() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Connection control
        void StartListening();
        void StopListening();
        void ToggleConnection();

        UDPConnectionState GetState() const { return m_state; }
        const UDPConfiguration& GetConfiguration() const { return m_config; }
        void SetConfiguration(const UDPConfiguration& config) { m_config = config; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderConnectionUI();
        void RenderConfigurationUI();
        void RenderStatusDisplay();

        UDPConfiguration m_config;
        UDPConnectionState m_state;
        bool m_configExpanded;
        std::string m_lastError;

        // Statistics
        int m_packetsReceived;
        int m_bytesReceived;
        float m_lastPacketTime;
    };

    // REST Connection Node - For HTTP REST API connections
    enum class RestConnectionState
    {
        Idle,
        Connecting,
        Connected,
        Requesting,
        Error
    };

    struct RestConfiguration
    {
        std::string baseUrl;
        std::vector<std::string> headers;
        std::string authMethod;     // "None", "Bearer", "ApiKey", "Basic"
        std::string authToken;
        int connectionTimeout;      // seconds
        int requestTimeout;         // seconds
        int maxRetries;
        bool enableCompression;

        RestConfiguration()
            : authMethod("None"), connectionTimeout(10), requestTimeout(30),
              maxRetries(3), enableCompression(true) {}
    };

    class RestConnectionNode : public Node
    {
    public:
        RestConnectionNode(ax::NodeEditor::NodeId nodeId);
        virtual ~RestConnectionNode() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Request methods
        void SendGetRequest(const std::string& endpoint);
        void SendPostRequest(const std::string& endpoint, const std::string& data);
        void SendPutRequest(const std::string& endpoint, const std::string& data);
        void SendDeleteRequest(const std::string& endpoint);

        RestConnectionState GetState() const { return m_state; }
        const RestConfiguration& GetConfiguration() const { return m_config; }
        void SetConfiguration(const RestConfiguration& config) { m_config = config; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderConnectionUI();
        void RenderConfigurationUI();
        void RenderRequestBuilder();
        void RenderStatusDisplay();

        RestConfiguration m_config;
        RestConnectionState m_state;
        bool m_configExpanded;
        bool m_requestBuilderExpanded;
        std::string m_lastError;

        // Request builder state
        char m_endpointBuffer[256];
        char m_requestDataBuffer[1024];
        int m_selectedMethod; // GET=0, POST=1, PUT=2, DELETE=3

        // Statistics
        int m_requestsSent;
        int m_responsesReceived;
        float m_averageResponseTime;
        int m_lastResponseCode;
    };
}