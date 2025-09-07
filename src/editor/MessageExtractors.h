#pragma once

#include "Node.h"
#include <chrono>
#include <unordered_map>

namespace gui::editor
{
    struct ExtractedMessageInfo
    {
        std::string messageId;
        std::chrono::system_clock::time_point timestamp;
        std::chrono::milliseconds ageMs;
        std::string originalMessage;
        std::string partiallyProcessedMessage;
        std::string messageType; // "Trade" or "Orderbook"
        bool isValid;
        
        ExtractedMessageInfo() : ageMs(0), isValid(false) {}
    };

    // Base class for message extractors
    class MessageExtractorBase : public Node
    {
    public:
        MessageExtractorBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                           const std::string& title);
        virtual ~MessageExtractorBase() = default;

        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Statistics
        int GetProcessedMessageCount() const { return m_processedCount; }
        int GetValidMessageCount() const { return m_validCount; }
        float GetAverageProcessingTime() const { return m_averageProcessingTime; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        virtual ExtractedMessageInfo ExtractMessageInfo(const std::string& message) = 0;
        void RenderStatistics();
        void RenderConfiguration();
        void ProcessIncomingMessages();
        
        // Configuration
        bool m_enabled;
        int m_maxMessageAge; // Maximum age in milliseconds to process
        bool m_validateTimestamps;
        
        // Statistics
        int m_processedCount;
        int m_validCount;
        int m_errorCount;
        float m_averageProcessingTime;
        
        // Message cache for deduplication
        std::unordered_map<std::string, std::chrono::system_clock::time_point> m_messageCache;
        int m_maxCacheSize;
        
    private:
        void CleanupMessageCache();
        void UpdateStatistics(float processingTime);
    };

    // FIX Message Trades Age Extractor
    class FixMessageTradesAgeExtractor : public MessageExtractorBase
    {
    public:
        FixMessageTradesAgeExtractor(ax::NodeEditor::NodeId nodeId);
        virtual ~FixMessageTradesAgeExtractor() = default;

        void Render() override;

    protected:
        ExtractedMessageInfo ExtractMessageInfo(const std::string& fixMessage) override;

    private:
        void RenderFixConfiguration();
        std::string ExtractFixField(const std::string& message, int tag);
        std::chrono::system_clock::time_point ParseFixTimestamp(const std::string& timestamp);
        
        // FIX-specific configuration
        std::string m_timestampTag; // Usually "52" for SendingTime
        std::string m_tradeIdTag;   // Usually "571" for TradeReportID
        std::string m_expectedMsgType; // Usually "AE" for TradeCapture
        bool m_strictValidation;
    };

    // JSON Message Trades Age Extractor  
    class JSONMessageTradesAgeExtractor : public MessageExtractorBase
    {
    public:
        JSONMessageTradesAgeExtractor(ax::NodeEditor::NodeId nodeId);
        virtual ~JSONMessageTradesAgeExtractor() = default;

        void Render() override;

    protected:
        ExtractedMessageInfo ExtractMessageInfo(const std::string& jsonMessage) override;

    private:
        void RenderJsonConfiguration();
        std::string ExtractJsonField(const std::string& message, const std::string& fieldPath);
        std::chrono::system_clock::time_point ParseJsonTimestamp(const std::string& timestamp);
        
        // JSON-specific configuration
        char m_timestampFieldPath[128]; // e.g., "data.timestamp" or "timestamp"
        char m_tradeIdFieldPath[128];   // e.g., "data.trade_id" or "id"
        char m_messageTypeFieldPath[128]; // e.g., "type" or "channel"
        char m_expectedMessageType[64];   // e.g., "trade" or "trades"
        bool m_timestampIsUnix; // true for Unix timestamp, false for ISO 8601
    };

    // FIX Message Orderbook Age Extractor
    class FixMessageOrderbookAgeExtractor : public MessageExtractorBase
    {
    public:
        FixMessageOrderbookAgeExtractor(ax::NodeEditor::NodeId nodeId);
        virtual ~FixMessageOrderbookAgeExtractor() = default;

        void Render() override;

    protected:
        ExtractedMessageInfo ExtractMessageInfo(const std::string& fixMessage) override;

    private:
        void RenderFixConfiguration();
        std::string ExtractFixField(const std::string& message, int tag);
        std::chrono::system_clock::time_point ParseFixTimestamp(const std::string& timestamp);
        
        // FIX-specific configuration for orderbook
        std::string m_timestampTag; // Usually "52" for SendingTime
        std::string m_mdReqIdTag;   // Usually "262" for MDReqID
        std::string m_expectedMsgType; // Usually "W" for MarketDataSnapshotFullRefresh
        bool m_strictValidation;
        int m_expectedMdEntryTypes; // Bit mask for expected entry types (0=Bid, 1=Offer)
    };

    // JSON Message Orderbook Age Extractor
    class JSONMessageOrderbookAgeExtractor : public MessageExtractorBase
    {
    public:
        JSONMessageOrderbookAgeExtractor(ax::NodeEditor::NodeId nodeId);
        virtual ~JSONMessageOrderbookAgeExtractor() = default;

        void Render() override;

    protected:
        ExtractedMessageInfo ExtractMessageInfo(const std::string& jsonMessage) override;

    private:
        void RenderJsonConfiguration();
        std::string ExtractJsonField(const std::string& message, const std::string& fieldPath);
        std::chrono::system_clock::time_point ParseJsonTimestamp(const std::string& timestamp);
        
        // JSON-specific configuration for orderbook
        char m_timestampFieldPath[128]; // e.g., "data.timestamp" or "timestamp"
        char m_orderbookIdFieldPath[128]; // e.g., "data.orderbook_id" or "id"
        char m_messageTypeFieldPath[128]; // e.g., "type" or "channel"
        char m_expectedMessageType[64];   // e.g., "orderbook" or "depth"
        bool m_timestampIsUnix;
        bool m_requiresBidsAndAsks; // Validate that both bids and asks are present
    };
}