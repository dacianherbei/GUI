//
// Created by dh on 03/09/2025.
//

#include "LoggingPanel.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace gui {
namespace panel {

LoggingPanel::LogEntry::LogEntry(const std::string& msg, LogLevel lvl):
    timestamp(LoggingPanel::LogEntry::LogEntry::CreateTimestamp()),
    level(lvl),
    message(msg)
{}

std::string LoggingPanel::LogEntry::LogEntry::CreateTimestamp() {
    // Generate timestamp
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

LoggingPanel::LoggingPanel()
    : m_autoScroll(true)
    , m_maxLogEntries(1000)
    , m_filterText{0}
    , m_showInfo(true)
    , m_showWarnings(true)
    , m_showErrors(true)
    , m_showSuccess(true)
{
    // memset(m_filterText, 0, sizeof(m_filterText));
}

LoggingPanel::~LoggingPanel()
{
    Shutdown();
}

bool LoggingPanel::Initialize()
{
    // Add some initial log entries
    AddLog("Logging system initialized", LogLevel::Success);
    AddLog("Ready to receive log messages", LogLevel::Info);

    return true;
}

void LoggingPanel::Render(bool* p_open)
{
    if (ImGui::Begin("Log Window", p_open))
    {
        RenderLogControls();
        ImGui::Separator();
        RenderLogEntries();
    }
    ImGui::End();
}

void LoggingPanel::RenderLogControls()
{
    // Filter controls
    ImGui::Text("Filters:");
    ImGui::SameLine();

    ImGui::Checkbox("Info", &m_showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &m_showWarnings);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &m_showErrors);
    ImGui::SameLine();
    ImGui::Checkbox("Success", &m_showSuccess);

    // Search filter
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputText("Search", m_filterText, sizeof(m_filterText));
    ImGui::SameLine();

    // Clear button
    if (ImGui::Button("Clear"))
    {
        ClearLog();
    }
    ImGui::SameLine();

    // Auto-scroll checkbox
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);

    ImGui::Text("Log entries: %zu", m_logEntries.size());
}

void LoggingPanel::RenderLogEntries()
{
    // Begin scrolling region
    if (ImGui::BeginChild("LogEntries", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGuiListClipper clipper;

        // Filter entries
        std::vector<const LogEntry*> filteredEntries;
        for (const auto& entry : m_logEntries)
        {
            // Apply level filters
            bool showLevel = false;
            switch (entry.level)
            {
                case LogLevel::Info: showLevel = m_showInfo; break;
                case LogLevel::Warning: showLevel = m_showWarnings; break;
                case LogLevel::Error: showLevel = m_showErrors; break;
                case LogLevel::Success: showLevel = m_showSuccess; break;
            }

            if (!showLevel)
                continue;

            // Apply text filter
            if (strlen(m_filterText) > 0)
            {
                std::string filterStr = m_filterText;
                std::string messageStr = entry.message;

                // Convert to lowercase for case-insensitive search
                std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);
                std::transform(messageStr.begin(), messageStr.end(), messageStr.begin(), ::tolower);

                if (messageStr.find(filterStr) == std::string::npos)
                    continue;
            }

            filteredEntries.push_back(&entry);
        }

        clipper.Begin(filteredEntries.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const LogEntry* entry = filteredEntries[i];

                // Color based on log level
                ImVec4 color = GetLogLevelColor(entry->level);

                // Render timestamp
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "[%s]", entry->timestamp.c_str());
                ImGui::SameLine();

                // Render level indicator
                const char* levelStr = "";
                switch (entry->level)
                {
                    case LogLevel::Info: levelStr = "INFO"; break;
                    case LogLevel::Warning: levelStr = "WARN"; break;
                    case LogLevel::Error: levelStr = "ERROR"; break;
                    case LogLevel::Success: levelStr = "OK"; break;
                }

                ImGui::TextColored(color, "[%s]", levelStr);
                ImGui::SameLine();

                // Render message
                ImGui::Text("%s", entry->message.c_str());
            }
        }

        // Auto-scroll to bottom
        if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}

ImVec4 LoggingPanel::GetLogLevelColor(LogLevel level) const
{
    switch (level)
    {
        case LogLevel::Info:
            return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        case LogLevel::Warning:
            return ImVec4(1.0f, 0.8f, 0.4f, 1.0f);
        case LogLevel::Error:
            return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        case LogLevel::Success:
            return ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
        default:
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void LoggingPanel::AddLog(const std::string& message, LogLevel level)
{
    m_logEntries.emplace_back(message, level);

    // Limit log entries
    if (m_logEntries.size() > m_maxLogEntries) {
        m_logEntries.erase(m_logEntries.begin());
    }
}

void LoggingPanel::ClearLog()
{
    m_logEntries.clear();
    AddLog("Log cleared", LogLevel::Info);
}

void LoggingPanel::Shutdown()
{
    // Cleanup if needed
}
} // namespace panel
} // namespace gui