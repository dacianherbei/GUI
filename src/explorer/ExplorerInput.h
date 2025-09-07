#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace gui {
namespace explorer {

// Forward declaration
class ExplorerDialog;

// File input component
class ExplorerInput
{
public:
    ExplorerInput(ExplorerDialog* parent);
    ~ExplorerInput();

    void Render();
    void SetSelectedFile(const std::string& filename);
    std::string GetInputFile() const;

private:
    ExplorerDialog* m_parent;
    char m_filenameInput[256];

    void OnInputChanged();
    void OnEnterPressed();
};

} // namespace explorer
} // namespace gui
