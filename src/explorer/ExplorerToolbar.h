#pragma once

#include "imgui.h"

#include <string>
#include <vector>

// Forward declaration
class ExplorerDialog;

namespace gui {
namespace explorer {

// Forward declaration
class ExplorerDialog;

// Toolbar component for navigation
class ExplorerToolbar
{
public:
    ExplorerToolbar(ExplorerDialog* parent);
    ~ExplorerToolbar();

    void Render();
    void SetCurrentDirectory(const std::string& directory);

private:
    ExplorerDialog* m_parent;
    std::string m_currentPath;
    std::vector<std::string> m_pathComponents;

    void NavigateToPath(const std::string& path);
    void GoUp();
    void GoHome();
    void RefreshView();
};

} // namespace explorer
} // namespace gui

