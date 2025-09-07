#pragma once

#include "Node.h"
#include "MessageExtractors.h"
#include <set>
#include <deque>
#include <unordered_set>

namespace gui::editor
{
    enum class FilterCriteria
    {
        ByAge,          // Filter by message age
        ById,           // Filter by message ID
        ByTimestamp,    // Filter by timestamp range
        ByDuplication,  // Filter duplicate messages
        ByExchange      // Filter by exchange source
    };

    struct FilterRule
    {
        FilterCriteria criteria;
        bool enabled;
        std::string value;        // Filter value (depends on criteria)
        std::string comparison;   // "greater", "less", "equal", "contains", "regex"
        int priority;             // Rule priority (lower number = higher priority)
        
        FilterRule() : criteria(FilterCriteria::ByAge), enabled(true), priority(0) {}
    };

    struct FilteredMessage
    {
        ExtractedMessageInfo messageInfo;
        std::string sourceConnection;
        std::string sourceExtractor;
        std::chrono::system_clock::time_point receivedTime;
        int filterScore; // Higher score = higher priority
        
        FilteredMessage() : filterScore(0) {}
    };

    // Base class for message filters
    class MessageFilterBase : public Node
    {
    public:
        MessageFilterBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                         const std::string& title, const std::string& messageType);
        virtual ~MessageFilterBase() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Filter management
        void AddFilterRule(const FilterRule& rule);
        void RemoveFilterRule(int index);
        void UpdateFilterRule(int index, const FilterRule& rule);
        const std::vector<FilterRule>& GetFilterRules() const { return m_filterRules; }

        // Statistics
        int GetProcessedMessageCount() const { return m_processedCount; }
        int GetFilteredMessageCount() const { return m_filteredCount; }
        int GetOutputMessageCount() const { return m_outputCount; }
        float GetFilterEfficiency() const;

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        void RenderFilterRules();
        void RenderStatistics();
        void RenderMessageQueue();
        void ProcessIncomingMessages();
        
        virtual bool ApplyCustomFilters(const FilteredMessage& message) = 0;
        virtual void ProcessFilteredMessage(const FilteredMessage& message) = 0;
        
        bool ApplyFilterRules(const FilteredMessage& message);
        bool ApplyFilterRule(const FilteredMessage& message, const FilterRule& rule);
        
        std::string m_messageType; // "Trade" or "Orderbook"
        std::vector<FilterRule> m_filterRules;
        
        // Message processing
        std::deque<FilteredMessage> m_messageQueue;
        std::unordered_set<std::string> m_processedIds; // For deduplication
        int m_maxQueueSize;
        int m_maxProcessedIdsCache;
        
        // Statistics
        int m_processedCount;
        int m_filteredCount;
        int m_outputCount;
        int m_duplicateCount;
        float m_averageProcessingTime;
        
        // Configuration
        bool m_enableDeduplication;
        bool m_enablePriorityQueue;
        int m_maxMessageAge; // Max age in milliseconds
        bool m_strictTimestampOrdering;

    private:
        void CleanupProcessedIds();
        void SortMessageQueue();
        void UpdateStatistics(float processingTime, bool filtered, bool output);
    };

    // Trades Filter Node
    class TradesFilter : public MessageFilterBase
    {
    public:
        TradesFilter(ax::NodeEditor::NodeId nodeId);
        virtual ~TradesFilter() = default;

    protected:
        bool ApplyCustomFilters(const FilteredMessage& message) override;
        void ProcessFilteredMessage(const FilteredMessage& message) override;

    private:
        void RenderTradesConfiguration();
        bool ValidateTradeMessage(const FilteredMessage& message);
        
        // Trades-specific filtering
        struct TradesFilterConfig
        {
            double minTradeSize;
            double maxTradeSize;
            double minPrice;
            double maxPrice;
            std::set<std::string> allowedCurrencyPairs;
            std::set<std::string> blockedCurrencyPairs;
            bool filterByVolume;
            bool filterByPrice;
            bool filterByCurrencyPair;
            
            TradesFilterConfig() 
                : minTradeSize(0.0), maxTradeSize(0.0), minPrice(0.0), maxPrice(0.0)
                , filterByVolume(false), filterByPrice(false), filterByCurrencyPair(false) {}
        } m_tradesConfig;
        
        // UI state
        char m_allowedPairsBuffer[256];
        char m_blockedPairsBuffer[256];
        bool m_configExpanded;
        
        // Trades-specific statistics
        int m_totalTradesProcessed;
        double m_totalVolumeProcessed;
        double m_averageTradeSize;
        std::string m_mostActivePair;
    };

    // Orderbook Filter Node
    class OrderbookFilter : public MessageFilterBase
    {
    public:
        OrderbookFilter(ax::NodeEditor::NodeId nodeId);
        virtual ~OrderbookFilter() = default;

    protected:
        bool ApplyCustomFilters(const FilteredMessage& message) override;
        void ProcessFilteredMessage(const FilteredMessage& message) override;

    private:
        void RenderOrderbookConfiguration();
        bool ValidateOrderbookMessage(const FilteredMessage& message);
        
        // Orderbook-specific filtering
        struct OrderbookFilterConfig
        {
            int minLevels;           // Minimum number of price levels
            int maxLevels;           // Maximum number of price levels  
            double minSpread;        // Minimum bid-ask spread
            double maxSpread;        // Maximum bid-ask spread
            bool requireFullBook;    // Require both bids and asks
            bool filterByLevels;
            bool filterBySpread;
            std::set<std::string> allowedCurrencyPairs;
            std::set<std::string> blockedCurrencyPairs;
            bool filterByCurrencyPair;
            
            OrderbookFilterConfig()
                : minLevels(1), maxLevels(100), minSpread(0.0), maxSpread(0.0)
                , requireFullBook(true), filterByLevels(false), filterBySpread(false)
                , filterByCurrencyPair(false) {}
        } m_orderbookConfig;
        
        // UI state
        char m_allowedPairsBuffer[256];
        char m_blockedPairsBuffer[256];
        bool m_configExpanded;
        
        // Orderbook-specific statistics
        int m_totalOrderbooksProcessed;
        int m_averageLevelsPerUpdate;
        double m_averageSpread;
        std::string m_mostActivePair;
        
        // Spread monitoring
        struct SpreadInfo
        {
            double currentSpread;
            double minSpread;
            double maxSpread;
            std::chrono::system_clock::time_point lastUpdate;
        };
        
        std::unordered_map<std::string, SpreadInfo> m_spreadInfo; // Per currency pair
        void UpdateSpreadInfo(const std::string& pair, double spread);
    };
}