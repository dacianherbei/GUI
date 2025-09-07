#pragma once

#include "imgui.h"
#include <string>
#include <vector>

namespace gui {
namespace panel {

class LoggingPanel
{
public:
    enum class LogLevel
    {
        Info,
        Warning,
        Error,
        Success
    };

    struct LogEntry
    {
        std::string timestamp;
        LogLevel level;
        std::string message;

        LogEntry(const std::string& msg, LogLevel lvl = LogLevel::Info);
        static std::string CreateTimestamp();
    };

    LoggingPanel();
    ~LoggingPanel();

    bool Initialize();
    void Render(bool* p_open = nullptr);
    void Shutdown();

    // Logging functions
    void AddLog(const std::string& message, LogLevel level = LogLevel::Info);
    void ClearLog();

    // Settings
    void SetAutoScroll(bool autoScroll) { m_autoScroll = autoScroll; }
    void SetMaxLogEntries(int maxEntries) { m_maxLogEntries = maxEntries; }

private:
    ImVec4 GetLogLevelColor(LogLevel level) const;
    void RenderLogControls();
    void RenderLogEntries();

    std::vector<LogEntry> m_logEntries;
    bool m_autoScroll;
    size_t m_maxLogEntries;

    // UI state
    char m_filterText[256];
    bool m_showInfo;
    bool m_showWarnings;
    bool m_showErrors;
    bool m_showSuccess;
};
} // namespace panel
} // namespace gui
