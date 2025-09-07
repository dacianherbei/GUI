//
// Created by dh on 03/09/2025.
//

#include "ExplorerDialog.h"

#include "ExplorerFavourites.h"
#include "ExplorerFolderView.h"
#include "ExplorerInput.h"
#include "ExplorerToolbar.h"

#include <stdio.h>

namespace gui {
namespace explorer {

ExplorerDialog::ExplorerDialog()
    : m_shouldShow(false)
    , m_action(Action::None)
    , m_currentDirectory(".")
    , m_fileFilter("*.*")
{
}

ExplorerDialog::~ExplorerDialog()
{
    Shutdown();
}

bool ExplorerDialog::Initialize()
{
    // Create components
    m_toolbar = std::make_unique<ExplorerToolbar>(this);
    m_favourites = std::make_unique<ExplorerFavourites>(this);
    m_folderView = std::make_unique<ExplorerFolderView>(this);
    m_input = std::make_unique<ExplorerInput>(this);

    // Set initial directory
    SetInitialDirectory(".");

    return true;
}

void ExplorerDialog::Render()
{
    if (!m_shouldShow)
        return;

    // Center the dialog
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(800, 600);
    ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;

    if (ImGui::Begin("Load Exchange Configuration", nullptr, flags))
    {
        RenderLayout();
    }
    ImGui::End();
}

void ExplorerDialog::RenderLayout()
{
    // Toolbar
    if (m_toolbar)
    {
        m_toolbar->Render();
        ImGui::Separator();
    }

    // Main content area
    if (ImGui::BeginChild("MainContent", ImVec2(0, -60), true))
    {
        // Split horizontally: favourites on left, folder view on right
        float favouritesWidth = 200.0f;

        // Favourites panel
        if (ImGui::BeginChild("Favourites", ImVec2(favouritesWidth, 0), true))
        {
            if (m_favourites)
                m_favourites->Render();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Folder view panel
        if (ImGui::BeginChild("FolderView", ImVec2(0, 0), true))
        {
            if (m_folderView)
                m_folderView->Render();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::Separator();

    // Input area
    if (m_input)
    {
        m_input->Render();
    }

    // Buttons
    ImGui::Spacing();
    float buttonWidth = 100.0f;
    float availWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(availWidth - (buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x));

    if (ImGui::Button("Accept", ImVec2(buttonWidth, 0)))
    {
        m_selectedFile = m_input->GetInputFile();
        if (!m_selectedFile.empty())
        {
            m_action = Action::Accept;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
    {
        m_action = Action::Cancel;
    }
}

void ExplorerDialog::SetInitialDirectory(const std::string& directory)
{
    m_currentDirectory = directory;
    if (m_toolbar)
        m_toolbar->SetCurrentDirectory(directory);
    if (m_folderView)
        m_folderView->SetDirectory(directory);
}

void ExplorerDialog::OnFileSelected(const std::string& filename)
{
    m_selectedFile = filename;
    if (m_input)
        m_input->SetSelectedFile(filename);
}

void ExplorerDialog::OnDirectoryChanged(const std::string& directory)
{
    m_currentDirectory = directory;
    if (m_toolbar)
        m_toolbar->SetCurrentDirectory(directory);
    if (m_folderView)
        m_folderView->SetDirectory(directory);
}

void ExplorerDialog::Shutdown()
{
    m_toolbar.reset();
    m_favourites.reset();
    m_folderView.reset();
    m_input.reset();
}
} // namespace explorer
} // namespace gui