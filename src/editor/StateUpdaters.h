#pragma once

#include "Node.h"
#include <unordered_map>
#include <vector>

namespace gui::editor
{
    // Common data structures for state management
    struct OrderState
    {
        std::string orderId;
        std::string clientOrderId;
        std::string account;
        std::string exchange;
        std::string currencyPair;
        std::string side; // "buy" or "sell"
        std::string type; // "market", "limit", "stop", etc.
        std::string status; // "pending", "open", "filled", "cancelled", "rejected"
        double originalQuantity;
        double filledQuantity;
        double remainingQuantity;
        double price;
        double averageFillPrice;
        std::chrono::system_clock::time_point createTime;
        std::chrono::system_clock::time_point updateTime;
        std::chrono::system_clock::time_point lastFillTime;
        std::vector<std::string> fills; // Fill IDs
        std::string rejectReason;
        
        OrderState() : originalQuantity(0.0), filledQuantity(0.0), remainingQuantity(0.0), 
                      price(0.0), averageFillPrice(0.0) {}
    };

    struct WalletBalance
    {
        std::string currency;
        double available;
        double locked;
        double total;
        std::chrono::system_clock::time_point updateTime;
        
        WalletBalance() : available(0.0), locked(0.0), total(0.0) {}
    };

    struct WalletState
    {
        std::string account;
        std::string exchange;
        std::unordered_map<std::string, WalletBalance> balances;
        double totalValueUSD;
        std::chrono::system_clock::time_point updateTime;
        
        WalletState() : totalValueUSD(0.0) {}
    };

    struct InstrumentData
    {
        std::string symbol;
        std::string exchange;
        std::string baseAsset;
        std::string quoteAsset;
        double minOrderSize;
        double maxOrderSize;
        double tickSize;
        double lotSize;
        int pricePrecision;
        int quantityPrecision;
        bool tradingEnabled;
        std::string tradingStatus; // "TRADING", "HALT", "BREAK", etc.
        std::chrono::system_clock::time_point updateTime;
        
        InstrumentData() : minOrderSize(0.0), maxOrderSize(0.0), tickSize(0.0), lotSize(0.0),
                          pricePrecision(0), quantityPrecision(0), tradingEnabled(false) {}
    };

    // Base class for state updaters
    class StateUpdaterBase : public Node
    {
    public:
        StateUpdaterBase(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, 
                        const std::string& title, const std::string& connectionType, 
                        const std::string& dataType);
        virtual ~StateUpdaterBase() = default;

        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Statistics
        int GetUpdatesProcessed() const { return m_updatesProcessed; }
        int GetUpdateErrors() const { return m_updateErrors; }
        float GetAverageUpdateTime() const { return m_averageUpdateTime; }

        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    protected:
        void RenderConfiguration();
        void RenderStatistics();
        void RenderDataView();
        void ProcessIncomingUpdates();
        
        virtual void ProcessStateUpdate(const std::string& message) = 0;
        virtual void ValidateStateData() = 0;
        virtual void RenderSpecificConfiguration() = 0;
        virtual void RenderSpecificDataView() = 0;
        
        void UpdateStatistics(float updateTime, bool success);
        void AddError(const std::string& error);
        
        std::string m_connectionType; // "REST", "WebSocket", "FIX"
        std::string m_dataType;       // "Order", "Wallet", "Instrument"
        
        // Configuration
        bool m_enabled;
        bool m_validateUpdates;
        bool m_enableErrorLogging;
        int m_updateFrequency; // For REST connections (seconds)
        int m_maxRetries;
        
        // Statistics
        int m_updatesProcessed;
        int m_updateErrors;
        float m_averageUpdateTime;
        std::chrono::system_clock::time_point m_lastUpdate;
        
        // Error tracking
        std::vector<std::string> m_recentErrors;
        static constexpr int MAX_ERROR_COUNT = 20;

    private:
        void CleanupErrors();
    };

    // Order State Updaters
    class RestOrderStateUpdater : public StateUpdaterBase
    {
    public:
        RestOrderStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~RestOrderStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessRestOrderUpdate(const std::string& jsonResponse);
        void RequestOrderUpdate(const std::string& orderId = "");
        void ParseOrderFromJson(const std::string& json, OrderState& order);
        
        std::unordered_map<std::string, OrderState> m_orders;
        
        // REST-specific configuration
        char m_ordersEndpoint[256];     // e.g., "/api/v3/openOrders"
        char m_orderDetailEndpoint[256]; // e.g., "/api/v3/order"
        bool m_fetchAllOrders;          // Fetch all orders or specific ones
        bool m_includeOrderHistory;     // Include filled/cancelled orders
        
        // Polling configuration
        bool m_enablePolling;
        int m_pollingInterval; // seconds
        float m_lastPollTime;
        
        // UI state
        bool m_ordersExpanded;
        char m_orderIdFilter[64];
    };

    class WebsocketOrderStateUpdater : public StateUpdaterBase
    {
    public:
        WebsocketOrderStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~WebsocketOrderStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessWebsocketOrderUpdate(const std::string& jsonMessage);
        void SubscribeToOrderUpdates();
        void ParseOrderFromJson(const std::string& json, OrderState& order);
        
        std::unordered_map<std::string, OrderState> m_orders;
        
        // WebSocket-specific configuration
        char m_orderUpdateChannel[128]; // e.g., "executionReport" or "orders"
        bool m_subscribeToAllOrders;
        bool m_subscribeToFills;
        bool m_subscribeToRejections;
        
        // Subscription state
        bool m_isSubscribed;
        std::chrono::system_clock::time_point m_subscriptionTime;
        
        // UI state
        bool m_ordersExpanded;
        bool m_subscriptionExpanded;
    };

    class FIXOrderStateUpdater : public StateUpdaterBase
    {
    public:
        FIXOrderStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXOrderStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessFixOrderUpdate(const std::string& fixMessage);
        void ParseOrderFromFix(const std::string& fixMsg, OrderState& order);
        std::string GetFixField(const std::string& message, int tag);
        
        std::unordered_map<std::string, OrderState> m_orders;
        
        // FIX-specific configuration
        std::string m_expectedMsgType; // "8" for ExecutionReport
        bool m_processExecutionReports;
        bool m_processOrderCancelRejects;
        bool m_processOrderReplaceRejects;
        
        // FIX tag configuration
        struct FixOrderTags
        {
            int orderIdTag;         // 37
            int clientOrderIdTag;   // 11
            int orderStatusTag;     // 39
            int sideTag;            // 54
            int orderTypeTag;       // 40
            int orderQtyTag;        // 38
            int priceTag;           // 44
            int lastQtyTag;         // 32
            int lastPxTag;          // 31
            int avgPxTag;           // 6
            int cumQtyTag;          // 14
            int leavesQtyTag;       // 151
            
            FixOrderTags() : orderIdTag(37), clientOrderIdTag(11), orderStatusTag(39),
                           sideTag(54), orderTypeTag(40), orderQtyTag(38), priceTag(44),
                           lastQtyTag(32), lastPxTag(31), avgPxTag(6), cumQtyTag(14), leavesQtyTag(151) {}
        } m_fixTags;
        
        // UI state
        bool m_ordersExpanded;
        bool m_tagsExpanded;
    };

    // Wallet State Updaters
    class RestWalletStateUpdater : public StateUpdaterBase
    {
    public:
        RestWalletStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~RestWalletStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessRestWalletUpdate(const std::string& jsonResponse);
        void RequestWalletUpdate();
        void ParseWalletFromJson(const std::string& json);
        
        WalletState m_walletState;
        
        // REST-specific configuration
        char m_balancesEndpoint[256];   // e.g., "/api/v3/account"
        bool m_includeZeroBalances;
        bool m_calculateTotalValue;
        
        // Polling configuration
        bool m_enablePolling;
        int m_pollingInterval;
        float m_lastPollTime;
        
        // UI state
        bool m_balancesExpanded;
        char m_currencyFilter[16];
    };

    class WebsocketWalletStateUpdater : public StateUpdaterBase
    {
    public:
        WebsocketWalletStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~WebsocketWalletStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessWebsocketWalletUpdate(const std::string& jsonMessage);
        void SubscribeToWalletUpdates();
        void ParseWalletFromJson(const std::string& json);
        
        WalletState m_walletState;
        
        // WebSocket-specific configuration
        char m_balanceUpdateChannel[128]; // e.g., "balanceUpdate" or "account"
        bool m_subscribeToAllCurrencies;
        
        // UI state
        bool m_balancesExpanded;
        bool m_subscriptionExpanded;
    };

    class FIXWalletStateUpdater : public StateUpdaterBase
    {
    public:
        FIXWalletStateUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXWalletStateUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessFixWalletUpdate(const std::string& fixMessage);
        void ParseWalletFromFix(const std::string& fixMsg);
        
        WalletState m_walletState;
        
        // FIX-specific configuration
        std::string m_expectedMsgType; // "j" for BusinessMessageReject or custom message
        
        // UI state
        bool m_balancesExpanded;
    };

    // Instrument Data Updaters
    class RestInstrumentDataUpdater : public StateUpdaterBase
    {
    public:
        RestInstrumentDataUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~RestInstrumentDataUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessRestInstrumentUpdate(const std::string& jsonResponse);
        void RequestInstrumentUpdate();
        void ParseInstrumentFromJson(const std::string& json, InstrumentData& instrument);
        
        std::unordered_map<std::string, InstrumentData> m_instruments;
        
        // REST-specific configuration
        char m_instrumentsEndpoint[256]; // e.g., "/api/v3/exchangeInfo"
        bool m_fetchAllInstruments;
        bool m_includeInactiveInstruments;
        
        // Polling configuration
        bool m_enablePolling;
        int m_pollingInterval;
        float m_lastPollTime;
        
        // UI state
        bool m_instrumentsExpanded;
        char m_symbolFilter[32];
    };

    class WebsocketInstrumentDataUpdater : public StateUpdaterBase
    {
    public:
        WebsocketInstrumentDataUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~WebsocketInstrumentDataUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessWebsocketInstrumentUpdate(const std::string& jsonMessage);
        void SubscribeToInstrumentUpdates();
        void ParseInstrumentFromJson(const std::string& json, InstrumentData& instrument);
        
        std::unordered_map<std::string, InstrumentData> m_instruments;
        
        // WebSocket-specific configuration
        char m_instrumentUpdateChannel[128]; // e.g., "instrumentUpdate" or "symbols"
        bool m_subscribeToAllInstruments;
        bool m_subscribeToStatusChanges;
        
        // UI state
        bool m_instrumentsExpanded;
        bool m_subscriptionExpanded;
    };

    class FIXInstrumentDataUpdater : public StateUpdaterBase
    {
    public:
        FIXInstrumentDataUpdater(ax::NodeEditor::NodeId nodeId);
        virtual ~FIXInstrumentDataUpdater() = default;

    protected:
        void ProcessStateUpdate(const std::string& message) override;
        void ValidateStateData() override;
        void RenderSpecificConfiguration() override;
        void RenderSpecificDataView() override;

    private:
        void ProcessFixInstrumentUpdate(const std::string& fixMessage);
        void ParseInstrumentFromFix(const std::string& fixMsg, InstrumentData& instrument);
        void RequestSecurityDefinition();
        
        std::unordered_map<std::string, InstrumentData> m_instruments;
        
        // FIX-specific configuration
        std::string m_expectedMsgType; // "d" for SecurityDefinition
        bool m_requestSecurityDefinitions;
        
        // UI state
        bool m_instrumentsExpanded;
    };
}