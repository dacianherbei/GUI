#pragma once

#include "Node.h"
#include <filesystem>

namespace gui::editor
{
    struct AccountConfiguration
    {
        std::string accountId;
        std::string accountName;
        std::string exchange;
        std::string apiKey;
        std::string apiSecret;
        std::string passphrase;     // For some exchanges like Coinbase Pro
        std::string environment;    // "Production", "Sandbox", "Demo"
        
        // Trading limits
        double maxOrderSize;
        double maxPositionSize;
        double dailyLossLimit;
        
        // Connection settings
        std::string baseUrl;
        std::string websocketUrl;
        bool enableTrading;
        bool enableMarketData;
        
        // Metadata
        std::string description;
        std::string createdDate;
        std::string lastUsed;
        bool isActive;
        
        AccountConfiguration()
            : maxOrderSize(0.0), maxPositionSize(0.0), dailyLossLimit(0.0)
            , enableTrading(false), enableMarketData(true), isActive(true) {}
    };

    enum class LoaderState
    {
        Idle,
        Loading,
        Loaded,
        Error
    };

    class AccountConfigurationLoaderNode : public Node
    {
    public:
        AccountConfigurationLoaderNode(ax::NodeEditor::NodeId nodeId);
        virtual ~AccountConfigurationLoaderNode() = default;

        // Node interface
        void Render() override;
        void Update(float deltaTime) override;
        bool IsValid() const override;

        // File operations
        void LoadFromFile(const std::string& filePath);
        void LoadFromDirectory(const std::string& directoryPath);
        void ReloadConfiguration();
        void SaveConfiguration(const std::string& filePath);
        
        // Configuration management
        const std::vector<AccountConfiguration>& GetAccounts() const { return m_accounts; }
        void AddAccount(const AccountConfiguration& account);
        void RemoveAccount(const std::string& accountId);
        void UpdateAccount(const AccountConfiguration& account);
        
        // State management
        LoaderState GetState() const { return m_state; }
        const std::string& GetStateText() const;
        int GetAccountCount() const { return m_accounts.size(); }
        
        // File watching
        void EnableFileWatching(bool enable) { m_watchFile = enable; }
        bool IsFileWatchingEnabled() const { return m_watchFile; }

        // Serialization
        void Serialize(std::ostream& out) const override;
        void Deserialize(std::istream& in) override;

    private:
        void RenderFileSelection();
        void RenderLoadedAccounts();
        void RenderAccountDetails(const AccountConfiguration& account);
        void RenderControlButtons();
        void RenderStatusDisplay();
        
        void LoadFromJson(const std::string& filePath);
        void LoadFromXml(const std::string& filePath);
        void LoadFromCsv(const std::string& filePath);
        
        void SaveToJson(const std::string& filePath);
        void SaveToXml(const std::string& filePath);
        void SaveToCsv(const std::string& filePath);
        
        void CheckFileChanges();
        void HandleLoadError(const std::string& error);
        void ValidateAccount(const AccountConfiguration& account);
        
        std::string GetFileFormat(const std::string& filePath);
        ImVec4 GetStateColor() const;
        const char* GetStateIcon() const;

        LoaderState m_state;
        std::vector<AccountConfiguration> m_accounts;
        
        // File management
        std::string m_currentFilePath;
        std::string m_currentDirectory;
        std::filesystem::file_time_type m_lastFileTime;
        bool m_watchFile;
        
        // UI state
        char m_filePathBuffer[512];
        bool m_accountsExpanded;
        bool m_detailsExpanded;
        int m_selectedAccountIndex;
        
        // Error handling
        std::string m_lastError;
        std::vector<std::string> m_validationWarnings;
        
        // Statistics
        struct LoadingStats
        {
            int totalLoaded;
            int validAccounts;
            int invalidAccounts;
            float loadTime;
            std::string lastLoadTime;
            
            LoadingStats() : totalLoaded(0), validAccounts(0), 
                           invalidAccounts(0), loadTime(0.0f) {}
        };
        
        LoadingStats m_stats;
        
        // File format support
        enum class FileFormat
        {
            Unknown,
            JSON,
            XML,
            CSV,
            INI
        };
        
        FileFormat GetFileFormatEnum(const std::string& filePath);
        
        // Default account templates
        static std::vector<AccountConfiguration> GetDefaultTemplates();
        void CreateAccountFromTemplate(const std::string& templateName);
        
        // Encryption support for sensitive data
        bool m_encryptSensitiveData;
        std::string m_encryptionKey;
        
        std::string EncryptString(const std::string& plaintext);
        std::string DecryptString(const std::string& ciphertext);
        
        // Auto-save functionality
        bool m_autoSave;
        float m_autoSaveInterval; // seconds
        float m_lastAutoSave;
        
        void PerformAutoSave();
    };
}