#pragma once

#include "Node.h"
#include "AccountConfigurationLoaderNode.h"

namespace gui::editor
{
    enum class SelectionMode
    {
        Manual,         // User manually selects account
        Automatic,      // Auto-select based on criteria
        RoundRobin,     // Cycle through accounts
        LoadBalancing   // Select based on current load
    };

    struct SelectionCriteria
    {
        std::string exchangeFilter;
        std::string environmentFilter;  // "Production", "Sandbox", "Demo"
        bool requireTrading;
        bool requireMarketData;
        double minOrderSize;
        double minPositionSize;
        std::vector<std::string> excludeAccounts;
        
        SelectionCriteria() 
            : requireTrading(false), requireMarketData(false)
            , minOrderSize(0.0), minPositionSize(0.0) {}
    };

    class AccountSelectorNode : public Node
    {
    public:
        AccountSelectorNode(ax::NodeEditor::NodeId nodeId);
        virtual ~AccountSelectorNode() = default;

        // Node interface
        void Render() override;
        void Update(float deltaTime) override;
        void OnInputConnected(ax::NodeEditor::PinId pinId) override;
        void OnInputDisconnected(ax::NodeEditor::PinId pinId) override;
        bool IsValid() const override;

        // Account selection
        void SelectAccount(const std::string& accountId);
        void SelectAccountByIndex(int index);
        void SelectNextAccount();
        void SelectPreviousAccount();
        void ClearSelection();
        
        // Auto-selection
        void SetSelectionMode(SelectionMode mode) { m_selectionMode = mode; }
        SelectionMode GetSelectionMode() const { return m_selectionMode; }
        void SetSelectionCriteria(const SelectionCriteria& criteria) { m_criteria = criteria; }
        const SelectionCriteria& GetSelectionCriteria() const { return m_criteria; }
        
        // Current selection
        const AccountConfiguration* GetSelectedAccount() const;
        std::string GetSelectedAccountId() const { return m_selectedAccountId; }
        int GetSelectedAccountIndex() const { return m_selectedAccountIndex; }
        bool HasValidSelection() const;
        
        // Available accounts (from input)
        const std::vector<AccountConfiguration>& GetAvailableAccounts() const { return m_availableAccounts; }
        std::vector<AccountConfiguration> GetFilteredAccounts() const;
        int GetAvailableAccountCount() const { return m_availableAccounts.size(); }

        // Serialization
        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderAccountSelection();
        void RenderSelectionMode();
        void RenderSelectionCriteria();
        void RenderAccountList();
        void RenderSelectedAccountInfo();
        void RenderControlButtons();
        
        void UpdateAvailableAccounts();
        void UpdateSelection();
        void ApplyAutoSelection();
        void ApplyRoundRobinSelection();
        void ApplyLoadBalancingSelection();
        
        bool AccountMatchesCriteria(const AccountConfiguration& account) const;
        void ValidateSelection();
        void HandleSelectionChange();
        
        ImVec4 GetSelectionStateColor() const;
        const char* GetSelectionStateIcon() const;

        SelectionMode m_selectionMode;
        SelectionCriteria m_criteria;
        
        // Selection state
        std::string m_selectedAccountId;
        int m_selectedAccountIndex;
        std::vector<AccountConfiguration> m_availableAccounts;
        std::vector<int> m_filteredAccountIndices;
        
        // UI state
        bool m_criteriaExpanded;
        bool m_accountListExpanded;
        bool m_selectedAccountExpanded;
        
        // Selection criteria UI buffers
        char m_exchangeFilterBuffer[64];
        char m_environmentFilterBuffer[32];
        char m_excludeAccountsBuffer[256];
        
        // Auto-selection state
        float m_lastSelectionTime;
        int m_roundRobinIndex;
        float m_roundRobinInterval; // seconds between account switches
        
        // Load balancing data
        struct AccountLoadInfo
        {
            std::string accountId;
            int activeConnections;
            float lastUsageTime;
            double currentLoad; // 0.0 to 1.0
        };
        
        std::vector<AccountLoadInfo> m_loadInfo;
        
        void UpdateLoadBalancingInfo();
        int FindLeastLoadedAccount() const;
        
        // Account validation and health
        struct AccountHealth
        {
            bool isConnected;
            bool hasRecentErrors;
            float responseTime;
            int errorCount;
            std::string lastError;
        };
        
        std::vector<AccountHealth> m_accountHealth;
        
        void UpdateAccountHealth();
        bool IsAccountHealthy(int accountIndex) const;
        
        // Favorites and preferences
        std::vector<std::string> m_favoriteAccounts;
        std::string m_preferredExchange;
        
        void AddToFavorites(const std::string& accountId);
        void RemoveFromFavorites(const std::string& accountId);
        bool IsFavorite(const std::string& accountId) const;
        
        // History and statistics
        struct SelectionHistory
        {
            std::string accountId;
            std::string timestamp;
            SelectionMode mode;
            float duration; // how long it was selected
        };
        
        std::vector<SelectionHistory> m_selectionHistory;
        static constexpr int MAX_HISTORY_SIZE = 50;
        
        void AddToHistory(const std::string& accountId, SelectionMode mode);
        void UpdateCurrentSelectionDuration();
        
        // Input validation
        bool HasValidAccountInput() const;
        void ValidateInputData();
        
        // Error handling
        std::string m_lastError;
        std::vector<std::string> m_warnings;
        
        void ClearErrors() { m_lastError.clear(); m_warnings.clear(); }
        void AddWarning(const std::string& warning) { m_warnings.push_back(warning); }
    };
}