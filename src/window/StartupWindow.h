#pragma once

#include "imgui.h"

namespace gui::explorer {
    class ExplorerDialog;
}

namespace gui {
namespace window {

class StartupWindow
{
public:
    enum class Action
    {
        None,
        CreateNewExchange,
        LoadExchange,
        OpenConnection,
        Close
    };

    StartupWindow();
    ~StartupWindow();

    bool Initialize();
    void Render();
    void Shutdown();

    Action GetSelectedAction() const { return m_selectedAction; }
    void ResetAction() { m_selectedAction = Action::None; }

    bool ShouldShow() const { return m_shouldShow; }
    void Show() { m_shouldShow = true; }
    void Hide() { m_shouldShow = false; }

private:
    void RenderMainButtons();
    void RenderExplorerDialog();

    bool m_shouldShow;
    Action m_selectedAction;
    bool m_showExplorerDialog;

    // Explorer dialog for loading exchange configurations
    gui::explorer::ExplorerDialog* m_explorerDialog;
};
} // namespace window
} // namespace gui
