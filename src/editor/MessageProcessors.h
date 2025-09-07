#pragma once

#include "Node.h"
#include "MessageFilters.h"
#include <vector>

namespace gui::editor
{
    // Normalized data structures
    struct NormalizedTrade
    {
        std::string tradeId;
        std::string currencyPair;
        std::string exchange;
        double price;
        double quantity;
        std::string side; // "buy" or "sell"
        std::chrono::system_clock::time_point timestamp;
        std::chrono::system_clock::time_point receivedTime;
        std::string originalMessageId;
        std::string source; // Connection source identifier
        
        // Optional fields
        std::string orderId;
        std::string buyOrderId;
        std::string sellOrderId;
        double fee;
        std::string feeCurrency;
        bool isMaker;
        
        NormalizedTrade() : price(0.0), quantity(0.0), fee(0.0), isMaker(false) {}
    };

    struct NormalizedOrderbookLevel
    {
        double price;
        double quantity;
        int orderCount; // Number of orders at this level (if available)
        
        NormalizedOrderbookLevel() : price(0.0), quantity(0.0), orderCount(0) {}
    };

    struct NormalizedOrderbook
    {
        std::string orderbookId;
        std::string currencyPair;
        std::string exchange;
        std::vector<NormalizedOrderbookLevel> bids;
        std::vector<NormalizedOrderbookLevel> asks;
        std::chrono::system_clock::time_point timestamp;
        std::chrono::system_clock::time_point receivedTime;
        std::string originalMessageId;
        std::string source;
        bool isSnapshot; // true for full snapshot, false for incremental update
        
        // Market info
        double spread;
        double midPrice;
        int totalLevels;
        
        NormalizedOrderbook() : isSnapshot(false), spread(0.0), midPrice(0.0), totalLevels(0) {}
    };

    // Base class for message processors
    class MessageProcessorBase : public Node
    {
    public:
        MessageProcessorBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                           const std::string& title, const std::string& messageType, 
                           const std::string& format);
        virtual ~MessageProcessorBase() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Statistics
        int GetProcessedMessageCount() const { return m_processedCount; }
        int GetNormalizedMessageCount() const { return m_normalizedCount; }
        int GetErrorCount() const { return m_errorCount; }
        float GetAverageProcessingTime() const { return m_averageProcessingTime; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        void RenderConfiguration();
        void RenderStatistics();
        void RenderErrorLog();
        void ProcessIncomingMessages();
        
        virtual void ProcessMessage(const FilteredMessage& message) = 0;
        virtual void ValidateNormalizedData() = 0;
        
        void AddError(const std::string& error, const std::string& messageId = "");
        void UpdateStatistics(float processingTime, bool success);
        
        std::string m_messageType; // "Trade" or "Orderbook"
        std::string m_format;      // "JSON" or "FIX"
        
        // Configuration
        bool m_validateData;
        bool m_enableErrorLogging;
        bool m_strictMode; // Reject messages with any validation errors
        int m_maxErrorsBeforeDisable;
        
        // Statistics
        int m_processedCount;
        int m_normalizedCount;
        int m_errorCount;
        float m_averageProcessingTime;
        
        // Error handling
        struct ProcessingError
        {
            std::string timestamp;
            std::string messageId;
            std::string error;
            std::string messageSnippet;
        };
        
        std::vector<ProcessingError> m_errors;
        static constexpr int MAX_ERROR_LOG_SIZE = 100;
        
    private:
        void CleanupErrorLog();
    };

    // JSON Trade Processor
    class JSONTradeProcessor : public MessageProcessorBase
    {
    public:
        JSONTradeProcessor(ax::NodeEditor::NodeId nodeId);
        virtual ~JSONTradeProcessor() = default;

    protected:
        void ProcessMessage(const FilteredMessage& message) override;
        void ValidateNormalizedData() override;

    private:
        void RenderJsonTradeConfiguration();
        NormalizedTrade ParseJsonTrade(const std::string& jsonMessage);
        std::string ExtractJsonField(const std::string& json, const std::string& fieldPath);
        double ParsePrice(const std::string& priceStr);
        double ParseQuantity(const std::string& quantityStr);
        std::chrono::system_clock::time_point ParseTimestamp(const std::string& timestampStr);
        
        // JSON field mapping configuration
        struct JsonTradeMapping
        {
            char tradeIdField[64];
            char priceField[64];
            char quantityField[64];
            char sideField[64];
            char timestampField[64];
            char currencyPairField[64];
            char orderIdField[64];
            char feeField[64];
            char feeCurrencyField[64];
            char isMakerField[64];
            
            JsonTradeMapping() {
                strcpy(tradeIdField, "id");
                strcpy(priceField, "price");
                strcpy(quantityField, "quantity");
                strcpy(sideField, "side");
                strcpy(timestampField, "timestamp");
                strcpy(currencyPairField, "symbol");
                strcpy(orderIdField, "orderId");
                strcpy(feeField, "fee");
                strcpy(feeCurrencyField, "feeCurrency");
                strcpy(isMakerField, "isMaker");
            }
        } m_jsonMapping;
        
        // Validation rules
        double m_minPrice;
        double m_maxPrice;
        double m_minQuantity;
        double m_maxQuantity;
        bool m_requireOrderId;
        bool m_requireFeeInfo;
        
        // UI state
        bool m_mappingExpanded;
        bool m_validationExpanded;
    };

    // JSON Orderbook Processor
    class JSONOrderbookProcessor : public MessageProcessorBase
    {
    public:
        JSONOrderbookProcessor(ax::NodeEditor::NodeId nodeId);
        virtual ~JSONOrderbookProcessor() = default;

    protected:
        void ProcessMessage(const FilteredMessage& message) override;
        void ValidateNormalizedData() override;

    private:
        void RenderJsonOrderbookConfiguration();
        NormalizedOrderbook ParseJsonOrderbook(const std::string& jsonMessage);
        std::vector<NormalizedOrderbookLevel> ParseOrderbookLevels(const std::string& levelsJson);
        
        // JSON field mapping configuration
        struct JsonOrderbookMapping
        {
            char orderbookIdField[64];
            char currencyPairField[64];
            char timestampField[64];
            char bidsField[64];
            char asksField[64];
            char priceField[64];    // Within each level
            char quantityField[64]; // Within each level
            char countField[64];    // Order count within each level
            
            JsonOrderbookMapping() {
                strcpy(orderbookIdField, "id");
                strcpy(currencyPairField, "symbol");
                strcpy(timestampField, "timestamp");
                strcpy(bidsField, "bids");
                strcpy(asksField, "asks");
                strcpy(priceField, "0");      // Array index or field name
                strcpy(quantityField, "1");   // Array index or field name
                strcpy(countField, "count");
            }
        } m_jsonMapping;
        
        // Processing configuration
        bool m_calculateSpread;
        bool m_calculateMidPrice;
        bool m_sortLevels;
        int m_maxLevelsPerSide;
        double m_minLevelQuantity;
        
        // UI state
        bool m_mappingExpanded;
        bool m_configExpanded;
    };

    // FIX Trade Processor
    class FIXTradeProcessor : public MessageProcessorBase
    {
    public:
        FIXTradeProcessor(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXTradeProcessor() = default;

    protected:
        void ProcessMessage(const FilteredMessage& message) override;
        void ValidateNormalizedData() override;

    private:
        void RenderFixTradeConfiguration();
        NormalizedTrade ParseFixTrade(const std::string& fixMessage);
        std::string GetFixField(const std::string& message, int tag);
        std::string GetFixField(const std::string& message, const std::string& tagStr);
        
        // FIX tag mapping configuration
        struct FixTradeMapping
        {
            int tradeIdTag;      // Usually 571 (TradeReportID)
            int priceTag;        // Usually 31 (LastPx)
            int quantityTag;     // Usually 32 (LastQty)  
            int sideTag;         // Usually 54 (Side)
            int timestampTag;    // Usually 60 (TransactTime)
            int symbolTag;       // Usually 55 (Symbol)
            int orderIdTag;      // Usually 37 (OrderID)
            int execIdTag;       // Usually 17 (ExecID)
            int commissionTag;   // Usually 12 (Commission)
            
            FixTradeMapping() 
                : tradeIdTag(571), priceTag(31), quantityTag(32), sideTag(54)
                , timestampTag(60), symbolTag(55), orderIdTag(37), execIdTag(17)
                , commissionTag(12) {}
        } m_fixMapping;
        
        // FIX message type validation
        std::string m_expectedMsgType; // Usually "8" for ExecutionReport or "AE" for TradeCaptureReport
        bool m_strictMsgTypeValidation;
        
        // Side mapping (FIX uses numbers)
        std::string MapFixSide(const std::string& fixSide);
        
        // UI state
        bool m_mappingExpanded;
        bool m_msgTypeExpanded;
    };

    // FIX Orderbook Processor
    class FIXOrderbookProcessor : public MessageProcessorBase
    {
    public:
        FIXOrderbookProcessor(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXOrderbookProcessor() = default;

    protected:
        void ProcessMessage(const FilteredMessage& message) override;
        void ValidateNormalizedData() override;

    private:
        void RenderFixOrderbookConfiguration();
        NormalizedOrderbook ParseFixOrderbook(const std::string& fixMessage);
        void ParseFixMarketDataEntries(const std::string& message, NormalizedOrderbook& orderbook);
        
        // FIX tag mapping configuration
        struct FixOrderbookMapping
        {
            int mdReqIdTag;      // Usually 262 (MDReqID)
            int symbolTag;       // Usually 55 (Symbol)
            int timestampTag;    // Usually 60 (TransactTime)
            int noMdEntriesTag;  // Usually 268 (NoMDEntries)
            int mdEntryTypeTag;  // Usually 269 (MDEntryType: 0=Bid, 1=Offer)
            int mdEntryPxTag;    // Usually 270 (MDEntryPx)
            int mdEntrySizeTag;  // Usually 271 (MDEntrySize)
            int mdEntryPositionNoTag; // Usually 290 (MDEntryPositionNo)
            
            FixOrderbookMapping()
                : mdReqIdTag(262), symbolTag(55), timestampTag(60), noMdEntriesTag(268)
                , mdEntryTypeTag(269), mdEntryPxTag(270), mdEntrySizeTag(271)
                , mdEntryPositionNoTag(290) {}
        } m_fixMapping;
        
        // FIX message type validation
        std::string m_expectedMsgType; // Usually "W" for MarketDataSnapshotFullRefresh
        bool m_processIncrementalUpdates; // Handle "X" MarketDataIncrementalRefresh
        
        // UI state
        bool m_mappingExpanded;
        bool m_msgTypeExpanded;
    };
}