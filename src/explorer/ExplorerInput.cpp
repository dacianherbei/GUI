#include "ExplorerInput.h"

#include "ExplorerDialog.h"

#include <filesystem>

namespace gui {
namespace explorer {
// ExplorerInput implementation
ExplorerInput::ExplorerInput(ExplorerDialog* parent)
    : m_parent(parent)
{
    memset(m_filenameInput, 0, sizeof(m_filenameInput));
}

ExplorerInput::~ExplorerInput()
{
}

void ExplorerInput::Render()
{
    ImGui::Text("File name:");
    ImGui::SetNextItemWidth(-100.0f);

    if (ImGui::InputText("##filename", m_filenameInput, sizeof(m_filenameInput), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        OnEnterPressed();
    }

    if (ImGui::IsItemEdited())
    {
        OnInputChanged();
    }
}

void ExplorerInput::SetSelectedFile(const std::string& filename)
{
    std::filesystem::path filePath(filename);
    std::string name = filePath.filename().string();

    strncpy(m_filenameInput, name.c_str(), sizeof(m_filenameInput) - 1);
    m_filenameInput[sizeof(m_filenameInput) - 1] = '\0';
}

std::string ExplorerInput::GetInputFile() const
{
    return std::string(m_filenameInput);
}

void ExplorerInput::OnInputChanged()
{
    // TODO: Update preview or validation
}

void ExplorerInput::OnEnterPressed()
{
    // TODO: Trigger accept action
}
} // namespace explorer
} // namespace gui