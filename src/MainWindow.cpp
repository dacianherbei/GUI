#include "MainWindow.h"

namespace gui
{

MainWindow::MainWindow()
    : m_shouldClose(false)
    , m_showCreateNewDialog(false)
    , m_showLoadDialog(false)
    , m_showConnectDialog(false)
{
}

MainWindow::~MainWindow()
{
    Shutdown();
}

bool MainWindow::Initialize()
{
    // Setup ImGui configuration
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup style
    ImGui::StyleColorsDark();

    return true;
}

void MainWindow::Render()
{
    // Create main window to fill the entire display
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    // Window flags for main window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Main window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Exchange Application", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Render menu bar
    RenderMenuBar();

    // Main content area (without docking, using BeginChild instead)
    ImGui::BeginChild("MainContent", ImVec2(0, 0), false, ImGuiWindowFlags_None);

    ImGui::Text("Welcome to Exchange Application");
    ImGui::Text("Use the menu above to create, load, or connect to an exchange.");

    // Display some basic information
    ImGui::Separator();
    ImGui::Text("Application status: Ready");
    ImGui::Text("Time: %.1f seconds", ImGui::GetTime());

    ImGui::EndChild();

    // Handle modal dialogs
    if (m_showCreateNewDialog)
    {
        ImGui::OpenPopup("Create New Exchange");
        m_showCreateNewDialog = false;
    }

    if (m_showLoadDialog)
    {
        ImGui::OpenPopup("Load Exchange");
        m_showLoadDialog = false;
    }

    if (m_showConnectDialog)
    {
        ImGui::OpenPopup("Connect to Exchange");
        m_showConnectDialog = false;
    }

    // Modal dialogs
    if (ImGui::BeginPopupModal("Create New Exchange", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Create a new exchange configuration");
        ImGui::Separator();

        static char exchange_name[128] = "";
        ImGui::InputText("Exchange Name", exchange_name, sizeof(exchange_name));

        static char exchange_type[128] = "";
        ImGui::InputText("Exchange Type", exchange_type, sizeof(exchange_type));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            HandleCreateNewExchange();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Load Exchange", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Load an existing exchange configuration");
        ImGui::Separator();

        static char file_path[256] = "";
        ImGui::InputText("File Path", file_path, sizeof(file_path));
        ImGui::SameLine();
        if (ImGui::Button("Browse"))
        {
            // TODO: Implement file browser
        }

        ImGui::Separator();

        if (ImGui::Button("Load", ImVec2(120, 0)))
        {
            HandleLoadExchange();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Connect to Exchange", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Connect to a remote exchange");
        ImGui::Separator();

        static char host_address[128] = "localhost";
        ImGui::InputText("Host Address", host_address, sizeof(host_address));

        static int port = 8080;
        ImGui::InputInt("Port", &port);

        static bool use_ssl = false;
        ImGui::Checkbox("Use SSL/TLS", &use_ssl);

        ImGui::Separator();

        if (ImGui::Button("Connect", ImVec2(120, 0)))
        {
            HandleConnectToExchange();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void MainWindow::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Exchange"))
        {
            if (ImGui::MenuItem("Create New Exchange", "Ctrl+N"))
            {
                m_showCreateNewDialog = true;
            }

            if (ImGui::MenuItem("Load Exchange", "Ctrl+O"))
            {
                m_showLoadDialog = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Connect to Exchange", "Ctrl+C"))
            {
                m_showConnectDialog = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Ctrl+Q"))
            {
                m_shouldClose = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void MainWindow::HandleCreateNewExchange()
{
    // TODO: Implement create new exchange logic
    printf("Create New Exchange requested\n");
}

void MainWindow::HandleLoadExchange()
{
    // TODO: Implement load exchange logic
    printf("Load Exchange requested\n");
}

void MainWindow::HandleConnectToExchange()
{
    // TODO: Implement connect to exchange logic
    printf("Connect to Exchange requested\n");
}

void MainWindow::Shutdown()
{
    // Cleanup resources if needed
}

bool MainWindow::ShouldClose() const
{
    return m_shouldClose;
}
}
