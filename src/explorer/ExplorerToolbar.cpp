#include "ExplorerToolbar.h"

#include "ExplorerDialog.h"

#include <filesystem>
#include <cstring>

namespace gui {
namespace explorer {

// ExplorerToolbar implementation
ExplorerToolbar::ExplorerToolbar(ExplorerDialog* parent)
    : m_parent(parent)
{
}

ExplorerToolbar::~ExplorerToolbar()
{
}

void ExplorerToolbar::Render()
{
    if (ImGui::Button("🏠 Home"))
    {
        GoHome();
    }
    ImGui::SameLine();

    if (ImGui::Button("⬆️ Up"))
    {
        GoUp();
    }
    ImGui::SameLine();

    if (ImGui::Button("🔄 Refresh"))
    {
        RefreshView();
    }
    ImGui::SameLine();

    // Current path display
    ImGui::Text("Path: %s", m_currentPath.c_str());
}

void ExplorerToolbar::SetCurrentDirectory(const std::string& directory)
{
    m_currentPath = directory;

    // Split path into components for breadcrumb navigation
    m_pathComponents.clear();
    std::string temp = directory;
    size_t pos = 0;
    while ((pos = temp.find('/')) != std::string::npos) {
        m_pathComponents.push_back(temp.substr(0, pos));
        temp.erase(0, pos + 1);
    }
    if (!temp.empty()) {
        m_pathComponents.push_back(temp);
    }
}

void ExplorerToolbar::GoUp()
{
    std::filesystem::path currentPath(m_currentPath);
    if (currentPath.has_parent_path())
    {
        NavigateToPath(currentPath.parent_path().string());
    }
}

void ExplorerToolbar::GoHome()
{
    NavigateToPath(std::filesystem::current_path().string());
}

void ExplorerToolbar::RefreshView()
{
    if (m_parent)
    {
        m_parent->OnDirectoryChanged(m_currentPath);
    }
}

void ExplorerToolbar::NavigateToPath(const std::string& path)
{
    if (m_parent)
    {
        m_parent->OnDirectoryChanged(path);
    }
}

} // namespace explorer
} // namespace gui