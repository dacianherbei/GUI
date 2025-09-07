//
// Created by dh on 03/09/2025.
//

#include "ExplorerFolderView.h"

#include "ExplorerDialog.h"

#include <algorithm>
#include <filesystem>
#include <format>

namespace gui {
namespace explorer {

// In ExplorerFolderView.cpp:
static std::string FormatFileTime(const std::filesystem::file_time_type& ftime)
{
    try {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto time_t = std::chrono::system_clock::to_time_t(sctp);

        char buffer[100];
        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&time_t)) > 0) {
            return std::string(buffer);
        }
    }
    catch (...) {
        // Fall through to default
    }

    return "Unknown";
}

// ExplorerFolderView implementation
ExplorerFolderView::ExplorerFolderView(ExplorerDialog* parent)
    : m_parent(parent)
    , m_fileFilter("*.*")
{
}

ExplorerFolderView::~ExplorerFolderView()
{
}

void ExplorerFolderView::Render()
{
    // Table with columns: Name, Size, Modified
    if (ImGui::BeginTable("FileList", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableHeadersRow();

        for (const auto& entry : m_entries)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // Icon + name
            const char* icon = entry.isDirectory ? "📁" : "📄";
            bool isSelected = (m_selectedFile == entry.fullPath);

            if (ImGui::Selectable((std::string(icon) + " " + entry.name).c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
            {
                OnItemSelected(entry);
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                OnItemDoubleClicked(entry);
            }

            ImGui::TableSetColumnIndex(1);
            if (!entry.isDirectory)
            {
                ImGui::Text("%zu", entry.fileSize);
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", entry.lastModified.c_str());
        }

        ImGui::EndTable();
    }
}

void ExplorerFolderView::SetDirectory(const std::string& directory)
{
    m_currentDirectory = directory;
    RefreshDirectory();
}

void ExplorerFolderView::SetFileFilter(const std::string& filter)
{
    m_fileFilter = filter;
    RefreshDirectory();
}

void ExplorerFolderView::RefreshDirectory()
{
    m_entries.clear();

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(m_currentDirectory))
        {
            FileEntry fileEntry;
            fileEntry.name = entry.path().filename().string();
            fileEntry.fullPath = entry.path().string();
            fileEntry.isDirectory = entry.is_directory();

            if (!fileEntry.isDirectory && !MatchesFilter(fileEntry.name))
                continue;

            // Get file size and modification time
            if (!fileEntry.isDirectory)
            {
                try
                {
                    fileEntry.fileSize = std::filesystem::file_size(entry.path());
                    fileEntry.lastModified = FormatFileTime(std::filesystem::last_write_time(entry.path()));
                }
                catch (...)
                {
                    fileEntry.fileSize = 0;
                    fileEntry.lastModified = "Unknown";
                }
            }
            else
            {
                fileEntry.fileSize = 0;
                fileEntry.lastModified = "Directory";
            }

            m_entries.push_back(fileEntry);
        }

        // Sort: directories first, then files
        std::sort(m_entries.begin(), m_entries.end(), [](const FileEntry& a, const FileEntry& b) {
            if (a.isDirectory != b.isDirectory)
                return a.isDirectory > b.isDirectory;
            return a.name < b.name;
        });
    }
    catch (const std::exception& e)
    {
        printf("Error reading directory %s: %s\n", m_currentDirectory.c_str(), e.what());
    }
}

void ExplorerFolderView::OnItemDoubleClicked(const FileEntry& entry)
{
    if (entry.isDirectory)
    {
        if (m_parent)
        {
            m_parent->OnDirectoryChanged(entry.fullPath);
        }
    }
    else
    {
        // Select file and trigger accept action
        m_selectedFile = entry.fullPath;
        if (m_parent)
        {
            m_parent->OnFileSelected(entry.fullPath);
        }
    }
}

void ExplorerFolderView::OnItemSelected(const FileEntry& entry)
{
    if (!entry.isDirectory)
    {
        m_selectedFile = entry.fullPath;
        if (m_parent)
        {
            m_parent->OnFileSelected(entry.fullPath);
        }
    }
}

bool ExplorerFolderView::MatchesFilter(const std::string& filename) const
{
    if (m_fileFilter == "*.*" || m_fileFilter.empty())
        return true;

    // Simple pattern matching - just check extension
    if (m_fileFilter.starts_with("*."))
    {
        std::string extension = m_fileFilter.substr(1);
        return filename.ends_with(extension);
    }

    return true;
}
} // namespace explorer
} // namespace gui