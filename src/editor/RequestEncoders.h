#pragma once

#include "Node.h"
#include <unordered_map>
#include <vector>

namespace gui::editor
{
    enum class RequestType
    {
        // Trading requests
        NewOrder,
        CancelOrder,
        ReplaceOrder,
        MassCancel,
        
        // Market data requests
        MarketDataRequest,
        MarketDataUnsubscribe,
        SecurityDefinitionRequest,
        
        // Account requests
        AccountStatusRequest,
        PositionRequest,
        BalanceRequest,
        TradeHistoryRequest,
        
        // System requests
        Heartbeat,
        TestRequest,
        Logon,
        Logout,
        
        // Custom request
        Custom
    };

    struct EncodedRequest
    {
        std::string requestId;
        RequestType requestType;
        std::string encodedMessage;
        std::string messageFormat; // "FIX" or "JSON"
        std::unordered_map<std::string, std::string> parameters;
        std::chrono::system_clock::time_point timestamp;
        int priority; // 0 = highest priority
        bool requiresSignature;
        std::string targetConnection; // Which connection to send to
        
        EncodedRequest() : requestType(RequestType::Custom), priority(5), requiresSignature(true) {}
    };

    // Base class for request encoders
    class RequestEncoderBase : public Node
    {
    public:
        RequestEncoderBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                          const std::string& title, const std::string& format);
        virtual ~RequestEncoderBase() = default;

        void Render() override;
        void Update(float deltaTime) override;
        bool IsValid() const override;

        // Request management
        void CreateRequest(RequestType type);
        void ClearCurrentRequest();
        void SendCurrentRequest();
        
        // Template management
        void SaveAsTemplate(const std::string& templateName);
        void LoadTemplate(const std::string& templateName);
        void DeleteTemplate(const std::string& templateName);

        // Statistics
        int GetRequestsCreated() const { return m_requestsCreated; }
        int GetRequestsSent() const { return m_requestsSent; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        void RenderRequestTypeSelector();
        void RenderRequestBuilder();
        void RenderTemplateManager();
        void RenderStatistics();
        void RenderPreview();
        
        virtual void RenderSpecificRequestBuilder() = 0;
        virtual std::string EncodeCurrentRequest() = 0;
        virtual void ValidateCurrentRequest() = 0;
        virtual void LoadRequestTemplates() = 0;
        
        void UpdatePreview();
        void AddParameter(const std::string& key, const std::string& value);
        void RemoveParameter(const std::string& key);
        
        std::string m_format; // "FIX" or "JSON"
        RequestType m_currentRequestType;
        EncodedRequest m_currentRequest;
        
        // Request building state
        std::unordered_map<std::string, std::string> m_currentParameters;
        std::string m_previewText;
        bool m_requestValid;
        std::vector<std::string> m_validationErrors;
        
        // Templates
        std::unordered_map<std::string, EncodedRequest> m_templates;
        char m_templateNameBuffer[128];
        
        // Statistics
        int m_requestsCreated;
        int m_requestsSent;
        
        // UI state
        bool m_requestBuilderExpanded;
        bool m_templateManagerExpanded;
        bool m_previewExpanded;

    private:
        void ValidateCommonFields();
        void CleanupOldRequests();
    };

    // FIX Request Encoder
    class FIXRequestEncoder : public RequestEncoderBase
    {
    public:
        FIXRequestEncoder(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXRequestEncoder() = default;

    protected:
        void RenderSpecificRequestBuilder() override;
        std::string EncodeCurrentRequest() override;
        void ValidateCurrentRequest() override;
        void LoadRequestTemplates() override;

    private:
        void RenderFixTagEditor();
        void RenderCommonFixFields();
        void RenderOrderSpecificFields();
        void RenderMarketDataFields();
        void RenderAccountFields();
        
        std::string BuildFixMessage();
        void AddFixTag(int tag, const std::string& value);
        std::string GetFixTag(int tag) const;
        void RemoveFixTag(int tag);
        
        // FIX-specific request building
        struct FixRequestBuilder
        {
            std::unordered_map<int, std::string> tags;
            std::string msgType;
            int msgSeqNum;
            std::string senderCompID;
            std::string targetCompID;
            
            FixRequestBuilder() : msgSeqNum(1) {}
        } m_fixBuilder;
        
        // Common FIX templates for different request types
        void InitializeFixTemplates();
        std::unordered_map<RequestType, std::unordered_map<int, std::string>> m_fixTemplates;
        
        // FIX field definitions for UI
        struct FixFieldDef
        {
            int tag;
            std::string name;
            std::string description;
            bool required;
            std::vector<std::string> validValues; // For enum fields
        };
        
        std::vector<FixFieldDef> GetFieldsForRequestType(RequestType type);
        std::unordered_map<int, FixFieldDef> m_fixFieldDefs;
        
        // Order-specific fields
        char m_symbolBuffer[32];
        char m_sideBuffer[16];
        char m_orderTypeBuffer[16];
        char m_quantityBuffer[32];
        char m_priceBuffer[32];
        char m_clientOrderIdBuffer[64];
        
        // UI state
        bool m_commonFieldsExpanded;
        bool m_orderFieldsExpanded;
        bool m_marketDataFieldsExpanded;
        bool m_customTagsExpanded;
        
        // Custom tag editor
        int m_customTag;
        char m_customValue[256];
    };

    // JSON Request Encoder
    class JSONRequestEncoder : public RequestEncoderBase
    {
    public:
        JSONRequestEncoder(ax::NodeEditor::NodeId nodeId);
        virtual ~JSONRequestEncoder() = default;

    protected:
        void RenderSpecificRequestBuilder() override;
        std::string EncodeCurrentRequest() override;
        void ValidateCurrentRequest() override;
        void LoadRequestTemplates() override;

    private:
        void RenderJsonStructureEditor();
        void RenderCommonJsonFields();
        void RenderOrderSpecificFields();
        void RenderMarketDataFields();
        void RenderAccountFields();
        void RenderJsonPreview();
        
        std::string BuildJsonMessage();
        void SetJsonField(const std::string& path, const std::string& value);
        std::string GetJsonField(const std::string& path) const;
        void RemoveJsonField(const std::string& path);
        
        // JSON-specific request building
        struct JsonRequestBuilder
        {
            std::unordered_map<std::string, std::string> fields;
            std::string method;     // For REST requests
            std::string endpoint;   // For REST requests
            std::string channel;    // For WebSocket requests
            
            JsonRequestBuilder() {}
        } m_jsonBuilder;
        
        // JSON templates for different exchanges
        void InitializeJsonTemplates();
        std::unordered_map<std::string, std::unordered_map<RequestType, std::string>> m_exchangeTemplates;
        
        // Exchange-specific JSON formats
        char m_selectedExchange[64];
        std::vector<std::string> m_supportedExchanges;
        
        // JSON field definitions for UI
        struct JsonFieldDef
        {
            std::string path;
            std::string name;
            std::string description;
            std::string dataType; // "string", "number", "boolean", "array", "object"
            bool required;
            std::vector<std::string> validValues;
        };
        
        std::vector<JsonFieldDef> GetFieldsForRequestType(RequestType type, const std::string& exchange);
        
        // Order-specific fields
        char m_symbolBuffer[32];
        char m_sideBuffer[16];
        char m_typeBuffer[16];
        char m_quantityBuffer[32];
        char m_priceBuffer[32];
        char m_clientOrderIdBuffer[64];
        char m_timeInForceBuffer[16];
        
        // WebSocket-specific fields
        char m_idBuffer[64];        // Request ID for WebSocket
        char m_methodBuffer[32];    // Method name
        char m_channelBuffer[64];   // Channel/topic
        
        // REST-specific fields
        char m_httpMethodBuffer[16]; // GET, POST, PUT, DELETE
        char m_endpointBuffer[128];  // API endpoint
        
        // UI state
        bool m_commonFieldsExpanded;
        bool m_orderFieldsExpanded;
        bool m_marketDataFieldsExpanded;
        bool m_webSocketFieldsExpanded;
        bool m_restFieldsExpanded;
        bool m_customFieldsExpanded;
        
        // Custom field editor
        char m_customFieldPath[128];
        char m_customFieldValue[256];
        
        // JSON validation
        bool ValidateJsonStructure(const std::string& json);
        std::string FormatJson(const std::string& json);
    };
}