//
// Created by dh on 03/09/2025.
//

#include "StartupWindow.h"

#include "explorer/ExplorerDialog.h"

#include <stdio.h>

namespace gui {
namespace window {

StartupWindow::StartupWindow()
    : m_shouldShow(true)
    , m_selectedAction(Action::None)
    , m_showExplorerDialog(false)
    , m_explorerDialog(nullptr)
{
}

StartupWindow::~StartupWindow()
{
    Shutdown();
}

bool StartupWindow::Initialize()
{
    m_explorerDialog = new gui::explorer::ExplorerDialog();
    if (!m_explorerDialog->Initialize())
    {
        printf("Failed to initialize ExplorerDialog\n");
        return false;
    }

    // Set up file filter for exchange configurations
    m_explorerDialog->SetFileFilter("*.json");

    return true;
}

void StartupWindow::Render()
{
    if (!m_shouldShow)
        return;

    if (m_showExplorerDialog)
    {
        RenderExplorerDialog();
    }
    else
    {
        RenderMainButtons();
    }
}

void StartupWindow::RenderMainButtons()
{
    // Center the startup window
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(500, 400);
    ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

    if (ImGui::Begin("Exchange Application - Startup", nullptr, flags))
    {
        // Title and description
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font, could be larger
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Welcome to Exchange Application");
        ImGui::PopFont();

        ImGui::Separator();
        ImGui::Text("Choose an option to get started:");
        ImGui::Spacing();

        // Calculate button size and spacing
        float buttonWidth = 400.0f;
        float buttonHeight = 60.0f;
        float spacing = 20.0f;

        // Center buttons horizontally
        float buttonStartX = (windowSize.x - buttonWidth) * 0.5f;

        // Option 1: Create New Exchange
        ImGui::SetCursorPosX(buttonStartX);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        if (ImGui::Button("🔧 Create New Exchange Configuration", ImVec2(buttonWidth, buttonHeight)))
        {
            m_selectedAction = Action::CreateNewExchange;
        }
        ImGui::PopStyleColor(2);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Create a new exchange configuration using the node editor");
        }

        ImGui::Spacing();

        // Option 2: Load Exchange
        ImGui::SetCursorPosX(buttonStartX);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
        if (ImGui::Button("📁 Load Exchange Configuration", ImVec2(buttonWidth, buttonHeight)))
        {
            m_showExplorerDialog = true;
            m_explorerDialog->Show();
        }
        ImGui::PopStyleColor(2);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Load a previously saved exchange configuration file");
        }

        ImGui::Spacing();

        // Option 3: Connect to Exchange
        ImGui::SetCursorPosX(buttonStartX);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.6f, 0.3f, 1.0f));
        if (ImGui::Button("🌐 Connect to Exchange", ImVec2(buttonWidth, buttonHeight)))
        {
            m_selectedAction = Action::OpenConnection;
        }
        ImGui::PopStyleColor(2);

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Connect to a live exchange for trading");
        }

        ImGui::Spacing();
        ImGui::Spacing();

        // Exit button
        ImGui::SetCursorPosX(buttonStartX);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("❌ Exit", ImVec2(buttonWidth, 40)))
        {
            m_selectedAction = Action::Close;
        }
        ImGui::PopStyleColor(2);

        ImGui::Spacing();

        // Footer information
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Exchange Application v1.0");
    }
    ImGui::End();
}

void StartupWindow::RenderExplorerDialog()
{
    if (m_explorerDialog->ShouldShow())
    {
        m_explorerDialog->Render();

        auto action = m_explorerDialog->GetAction();
        switch (action)
        {
            case gui::explorer::ExplorerDialog::Action::Accept:
            {
                std::string selectedFile = m_explorerDialog->GetSelectedFile();
                printf("Loading exchange configuration: %s\n", selectedFile.c_str());

                // TODO: Actually load the configuration file
                m_selectedAction = Action::LoadExchange;

                m_explorerDialog->ResetAction();
                m_explorerDialog->Hide();
                m_showExplorerDialog = false;
                break;
            }

            case gui::explorer::ExplorerDialog::Action::Cancel:
            case gui::explorer::ExplorerDialog::Action::Back:
                m_explorerDialog->ResetAction();
                m_explorerDialog->Hide();
                m_showExplorerDialog = false;
                break;

            default:
                break;
        }
    }
}

void StartupWindow::Shutdown()
{
    if (m_explorerDialog)
    {
        m_explorerDialog->Shutdown();
        delete m_explorerDialog;
        m_explorerDialog = nullptr;
    }
}
} // namespace window
} // namespace gui