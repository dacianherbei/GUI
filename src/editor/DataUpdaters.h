#pragma once

#include "Node.h"
#include "MessageProcessors.h"
#include <queue>
#include <unordered_map>
#include <functional>

namespace gui::editor
{
    enum class UpdateOrder
    {
        ByTimestamp,    // Order by message timestamp
        BySequence,     // Order by sequence number/ID
        ByPriority,     // Order by assigned priority
        BySource        // Process by source order
    };

    enum class ConflictResolution
    {
        LatestWins,     // Latest timestamp wins
        SourcePriority, // Higher priority source wins
        ManualReview,   // Queue for manual resolution
        Merge           // Attempt to merge data
    };

    // Base class for data updaters
    class DataUpdaterBase : public Node
    {
    public:
        DataUpdaterBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                       const std::string& title, const std::string& dataType);
        virtual ~DataUpdaterBase() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Configuration
        void SetUpdateOrder(UpdateOrder order) { m_updateOrder = order; }
        UpdateOrder GetUpdateOrder() const { return m_updateOrder; }
        void SetConflictResolution(ConflictResolution resolution) { m_conflictResolution = resolution; }
        ConflictResolution GetConflictResolution() const { return m_conflictResolution; }

        // Statistics
        int GetProcessedCount() const { return m_processedCount; }
        int GetQueuedCount() const { return m_queuedCount; }
        int GetConflictCount() const { return m_conflictCount; }
        float GetAverageLatency() const { return m_averageLatency; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        void RenderConfiguration();
        void RenderStatistics();
        void RenderQueue();
        void RenderConflicts();
        
        virtual void ProcessQueuedUpdates() = 0;
        virtual void HandleDataConflict(const std::string& conflictInfo) = 0;
        virtual bool ShouldQueueUpdate(const std::string& updateId) = 0;
        
        void UpdateStatistics(float latency);
        void AddConflict(const std::string& conflictInfo);
        
        std::string m_dataType; // "Trade" or "Orderbook"
        
        // Configuration
        UpdateOrder m_updateOrder;
        ConflictResolution m_conflictResolution;
        int m_maxQueueSize;
        int m_maxWaitTime; // Maximum time to wait for missing updates (ms)
        bool m_enableConflictDetection;
        bool m_strictOrdering;
        
        // Source priority mapping
        std::unordered_map<std::string, int> m_sourcePriorities;
        
        // Statistics
        int m_processedCount;
        int m_queuedCount;
        int m_conflictCount;
        int m_outOfOrderCount;
        float m_averageLatency;
        
        // Conflict tracking
        struct DataConflict
        {
            std::string conflictId;
            std::string description;
            std::chrono::system_clock::time_point timestamp;
            std::vector<std::string> involvedSources;
            bool resolved;
        };
        
        std::vector<DataConflict> m_conflicts;
        static constexpr int MAX_CONFLICT_HISTORY = 50;

    private:
        void CleanupConflicts();
    };

    // Trades Updater Node
    class TradesUpdater : public DataUpdaterBase
    {
    public:
        TradesUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~TradesUpdater() = default;

    protected:
        void ProcessQueuedUpdates() override;
        void HandleDataConflict(const std::string& conflictInfo) override;
        bool ShouldQueueUpdate(const std::string& updateId) override;

    private:
        void RenderTradesConfiguration();
        void ProcessTradeUpdate(const NormalizedTrade& trade);
        bool ValidateTradeSequence(const NormalizedTrade& trade);
        void DetectTradeConflicts(const NormalizedTrade& trade);
        
        // Trade-specific update queue
        struct QueuedTrade
        {
            NormalizedTrade trade;
            std::chrono::system_clock::time_point queueTime;
            int priority;
            std::string sourceConnection;
            
            // Comparison operators for priority queue
            bool operator<(const QueuedTrade& other) const;
        };
        
        std::priority_queue<QueuedTrade> m_tradeQueue;
        
        // Trade sequence tracking
        std::unordered_map<std::string, uint64_t> m_lastTradeSequence; // Per currency pair
        std::unordered_map<std::string, std::chrono::system_clock::time_point> m_lastTradeTime; // Per currency pair
        
        // Duplicate detection
        std::unordered_set<std::string> m_processedTradeIds;
        int m_maxTradeIdCache;
        
        // Trade-specific configuration
        struct TradesUpdaterConfig
        {
            bool enableSequenceValidation;
            bool enableDuplicateDetection;
            bool enableTimestampValidation;
            bool allowOutOfOrderTrades;
            int maxSequenceGap;        // Maximum allowed gap in sequence numbers
            int maxTimestampSkew;      // Maximum allowed timestamp difference (ms)
            double conflictPriceThreshold; // Price difference threshold for conflicts (%)
            double conflictQuantityThreshold; // Quantity difference threshold (%)
            
            TradesUpdaterConfig()
                : enableSequenceValidation(true), enableDuplicateDetection(true)
                , enableTimestampValidation(true), allowOutOfOrderTrades(false)
                , maxSequenceGap(10), maxTimestampSkew(5000)
                , conflictPriceThreshold(0.01), conflictQuantityThreshold(0.01) {}
        } m_tradesConfig;
        
        // Performance tracking
        struct TradeMetrics
        {
            int tradesPerSecond;
            double averageTradeSize;
            double totalVolume;
            std::unordered_map<std::string, int> tradesByPair;
            std::unordered_map<std::string, int> tradesByExchange;
            
            TradeMetrics() : tradesPerSecond(0), averageTradeSize(0.0), totalVolume(0.0) {}
        } m_metrics;
        
        void UpdateTradeMetrics(const NormalizedTrade& trade);
        void CleanupProcessedTradeIds();
        
        // UI state
        bool m_configExpanded;
        bool m_metricsExpanded;
    };

    // Orderbook Updater Node
    class OrderbookUpdater : public DataUpdaterBase
    {
    public:
        OrderbookUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~OrderbookUpdater() = default;

    protected:
        void ProcessQueuedUpdates() override;
        void HandleDataConflict(const std::string& conflictInfo) override;
        bool ShouldQueueUpdate(const std::string& updateId) override;

    private:
        void RenderOrderbookConfiguration();
        void ProcessOrderbookUpdate(const NormalizedOrderbook& orderbook);
        bool ValidateOrderbookSequence(const NormalizedOrderbook& orderbook);
        void DetectOrderbookConflicts(const NormalizedOrderbook& orderbook);
        void MergeOrderbookUpdates(const NormalizedOrderbook& update);
        
        // Orderbook-specific update queue
        struct QueuedOrderbook
        {
            NormalizedOrderbook orderbook;
            std::chrono::system_clock::time_point queueTime;
            int priority;
            std::string sourceConnection;
            
            // Comparison operators for priority queue
            bool operator<(const QueuedOrderbook& other) const;
        };
        
        std::priority_queue<QueuedOrderbook> m_orderbookQueue;
        
        // Current orderbook state (for incremental updates)
        std::unordered_map<std::string, NormalizedOrderbook> m_currentOrderbooks; // Per currency pair
        
        // Sequence tracking
        std::unordered_map<std::string, uint64_t> m_lastOrderbookSequence; // Per currency pair
        std::unordered_map<std::string, std::chrono::system_clock::time_point> m_lastOrderbookTime; // Per currency pair
        
        // Orderbook-specific configuration
        struct OrderbookUpdaterConfig
        {
            bool enableSequenceValidation;
            bool enableSnapshotRecovery;    // Request snapshot when sequence gap detected
            bool enableIncrementalUpdates;
            bool validateOrderbookIntegrity; // Check bid <= ask, sorted levels, etc.
            int maxSequenceGap;
            int maxTimestampSkew;
            int snapshotRequestThreshold;   // Gap size that triggers snapshot request
            double conflictSpreadThreshold; // Spread difference threshold for conflicts (%)
            bool maintainLevelHistory;      // Keep history of level changes
            
            OrderbookUpdaterConfig()
                : enableSequenceValidation(true), enableSnapshotRecovery(true)
                , enableIncrementalUpdates(true), validateOrderbookIntegrity(true)
                , maxSequenceGap(5), maxTimestampSkew(2000), snapshotRequestThreshold(3)
                , conflictSpreadThreshold(1.0), maintainLevelHistory(false) {}
        } m_orderbookConfig;
        
        // Orderbook integrity validation
        bool ValidateOrderbookIntegrity(const NormalizedOrderbook& orderbook);
        bool ValidateLevelSorting(const std::vector<NormalizedOrderbookLevel>& levels, bool isBids);
        double CalculateSpread(const NormalizedOrderbook& orderbook);
        
        // Performance tracking
        struct OrderbookMetrics
        {
            int updatesPerSecond;
            int snapshotsReceived;
            int incrementalUpdatesReceived;
            double averageSpread;
            std::unordered_map<std::string, int> updatesByPair;
            std::unordered_map<std::string, double> spreadByPair;
            
            OrderbookMetrics() 
                : updatesPerSecond(0), snapshotsReceived(0)
                , incrementalUpdatesReceived(0), averageSpread(0.0) {}
        } m_metrics;
        
        void UpdateOrderbookMetrics(const NormalizedOrderbook& orderbook);
        
        // Gap detection and recovery
        struct SequenceGap
        {
            std::string currencyPair;
            uint64_t expectedSequence;
            uint64_t receivedSequence;
            std::chrono::system_clock::time_point detectedTime;
            bool recoveryRequested;
        };
        
        std::vector<SequenceGap> m_detectedGaps;
        void HandleSequenceGap(const std::string& pair, uint64_t expected, uint64_t received);
        void RequestSnapshot(const std::string& pair);
        
        // UI state
        bool m_configExpanded;
        bool m_metricsExpanded;
        bool m_integrityExpanded;
    };
}