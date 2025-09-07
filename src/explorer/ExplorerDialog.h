#pragma once

#include "imgui.h"

#include <string>
#include <memory>

namespace gui {
namespace explorer {

class ExplorerToolbar;
class ExplorerFavourites;
class ExplorerFolderView;
class ExplorerInput;

class ExplorerDialog
{
public:
    enum class Action
    {
        None,
        Accept,
        Cancel,
        Back
    };

    ExplorerDialog();
    ~ExplorerDialog();

    bool Initialize();
    void Render();
    void Shutdown();

    Action GetAction() const { return m_action; }
    void ResetAction() { m_action = Action::None; }

    std::string GetSelectedFile() const { return m_selectedFile; }

    bool ShouldShow() const { return m_shouldShow; }
    void Show() { m_shouldShow = true; }
    void Hide() { m_shouldShow = false; }

    // Configuration
    void SetFileFilter(const std::string& filter) { m_fileFilter = filter; }
    void SetInitialDirectory(const std::string& directory);

private:
    void RenderLayout();
    void OnFileSelected(const std::string& filename);
    void OnDirectoryChanged(const std::string& directory);

    bool m_shouldShow;
    Action m_action;
    std::string m_selectedFile;
    std::string m_currentDirectory;
    std::string m_fileFilter;

    // Components
    std::unique_ptr<ExplorerToolbar> m_toolbar;
    std::unique_ptr<ExplorerFavourites> m_favourites;
    std::unique_ptr<ExplorerFolderView> m_folderView;
    std::unique_ptr<ExplorerInput> m_input;

    friend class ExplorerToolbar;
    friend class ExplorerFolderView;
    friend class ExplorerInput;
    friend class ExplorerFavourites;
};

} // namespace explorer
} // namespace gui