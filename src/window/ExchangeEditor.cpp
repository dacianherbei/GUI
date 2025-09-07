#include "ExchangeEditor.h"

#include <algorithm>
#include <stdio.h>
#include <fstream>

#include "imgui_node_editor.h"

using namespace std;

namespace gui {
namespace window {

ExchangeEditor::ExchangeEditor():
    m_editorContext(ax::NodeEditor::EditorContextPtr(ax::NodeEditor::CreateEditor(),ax::NodeEditor::DestroyEditor))
    , m_shouldShow(false)
    , m_action(Action::None)
    , m_nextId(1)
    , m_selectedNodeId(-1)
    , m_showPropertiesPanel(true)
    , m_showNodePalette(true)
{}

ExchangeEditor::~ExchangeEditor()
{
    Shutdown();
}

bool ExchangeEditor::Initialize()
{
    // TODO: Initialize imgui-node-editor context
    // m_editorContext = ax::NodeEditor::CreateEditor();

    // Initialize available node types
    m_nodeTypes = {
        "Exchange Connection",
        "Data Feed",
        "Price Filter",
        "Volume Filter",
        "Moving Average",
        "RSI Indicator",
        "MACD Indicator",
        "Bollinger Bands",
        "Order Placement",
        "Risk Management",
        "Position Sizing",
        "Stop Loss",
        "Take Profit",
        "Alert/Notification",
        "Logger"
    };

    printf("ExchangeEditor initialized (Node editor integration pending)\n");
    return true;
}

void ExchangeEditor::Render()
{
    if (!m_shouldShow)
        return;

    // Create fullscreen window for editor
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Exchange Configuration Editor", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    RenderMenuBar();

    // Main editor layout
    if (ImGui::BeginChild("EditorContent", ImVec2(0, 0)))
    {
        // Split layout: Node palette on left, editor in center, properties on right
        float paletteWidth = m_showNodePalette ? 200.0f : 0.0f;
        float propertiesWidth = m_showPropertiesPanel ? 250.0f : 0.0f;

        if (m_showNodePalette)
        {
            RenderNodePalette();
            ImGui::SameLine();
        }

        // Node editor area
        float editorWidth = ImGui::GetContentRegionAvail().x;
        if (m_showPropertiesPanel)
            editorWidth -= propertiesWidth + ImGui::GetStyle().ItemSpacing.x;

        if (ImGui::BeginChild("NodeEditor", ImVec2(editorWidth, 0), true))
        {
            RenderNodeEditor();
        }
        ImGui::EndChild();

        if (m_showPropertiesPanel)
        {
            ImGui::SameLine();
            RenderPropertiesPanel();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void ExchangeEditor::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                // Clear current configuration
                m_nodes.clear();
                m_pins.clear();
                m_links.clear();
                m_nextId = 1;
                printf("New exchange configuration created\n");
            }

            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                // TODO: Open file dialog
                printf("Open configuration dialog would appear here\n");
            }

            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                m_action = Action::Save;
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
            {
                // TODO: Save As dialog
                printf("Save As dialog would appear here\n");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Back to Startup"))
            {
                m_action = Action::Back;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
                // TODO: Implement undo
            }

            if (ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
                // TODO: Implement redo
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Delete Selected", "Del"))
            {
                if (m_selectedNodeId >= 0)
                {
                    DeleteNode(m_selectedNodeId);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Node Palette", nullptr, &m_showNodePalette);
            ImGui::MenuItem("Properties Panel", nullptr, &m_showPropertiesPanel);

            ImGui::Separator();

            if (ImGui::MenuItem("Zoom to Fit"))
            {
                // TODO: Implement zoom to fit
            }

            if (ImGui::MenuItem("Reset Zoom"))
            {
                // TODO: Implement reset zoom
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Node Editor Guide"))
            {
                // TODO: Show help
            }

            if (ImGui::MenuItem("About"))
            {
                // TODO: Show about
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void ExchangeEditor::RenderNodeEditor()
{
    ImGui::Text("Node Editor Area");
    ImGui::Separator();

    // TODO: Replace this with actual imgui-node-editor rendering
    // ax::NodeEditor::SetCurrentEditor(m_editorContext.get());
    // ax::NodeEditor::Begin("Node Editor");

    // Placeholder content
    ImGui::Text("📝 Exchange Configuration Node Editor");
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "This area will contain the node editor interface where you can:");
    ImGui::BulletText("Drag nodes from the palette on the left");
    ImGui::BulletText("Connect nodes together to create trading logic");
    ImGui::BulletText("Configure node properties in the panel on the right");
    ImGui::BulletText("Save and load exchange configurations");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Node Editor Integration Status:");
    ImGui::Text("⏳ Pending imgui-node-editor library integration");

    ImGui::Spacing();
    ImGui::Text("Current Configuration:");
    ImGui::Text("Nodes: %zu", m_nodes.size());
    ImGui::Text("Links: %zu", m_links.size());

    // Temporary buttons for testing
    if (ImGui::Button("Add Test Node"))
    {
        CreateNode("Test Node", ImVec2(100, 100));
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear All"))
    {
        m_nodes.clear();
        m_pins.clear();
        m_links.clear();
        m_nextId = 1;
        m_selectedNodeId = -1;
    }

    // TODO: End node editor rendering
    // ax::NodeEditor::End();
    // ax::NodeEditor::SetCurrentEditor(nullptr);
}

void ExchangeEditor::RenderNodePalette()
{
    if (ImGui::BeginChild("NodePalette", ImVec2(200, 0), true))
    {
        ImGui::Text("Node Palette");
        ImGui::Separator();

        for (const auto& nodeType : m_nodeTypes)
        {
            if (ImGui::Selectable(nodeType.c_str()))
            {
                // TODO: Start drag operation or create node at center
                CreateNode(nodeType, ImVec2(300, 200)); // Placeholder position
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Click to add %s node", nodeType.c_str());
            }
        }
    }
    ImGui::EndChild();
}

void ExchangeEditor::RenderPropertiesPanel()
{
    if (ImGui::BeginChild("PropertiesPanel", ImVec2(250, 0), true))
    {
        ImGui::Text("Properties");
        ImGui::Separator();

        if (m_selectedNodeId >= 0)
        {
            // Find selected node
            Node* selectedNode = nullptr;
            for (auto& node : m_nodes)
            {
                if (node.id == m_selectedNodeId)
                {
                    selectedNode = &node;
                    break;
                }
            }

            if (selectedNode)
            {
                ImGui::Text("Node: %s", selectedNode->name.c_str());
                ImGui::Text("Type: %s", selectedNode->type.c_str());
                ImGui::Text("ID: %d", selectedNode->id);

                ImGui::Separator();

                // Editable properties
                char nameBuffer[256];
                strncpy(nameBuffer, selectedNode->name.c_str(), sizeof(nameBuffer) - 1);
                nameBuffer[sizeof(nameBuffer) - 1] = '\0';

                if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
                {
                    selectedNode->name = nameBuffer;
                }

                ImGui::Text("Position: (%.1f, %.1f)", selectedNode->position.x, selectedNode->position.y);

                // TODO: Add type-specific properties based on node type
                if (selectedNode->type == "Exchange Connection")
                {
                    static char exchangeName[128] = "Binance";
                    ImGui::InputText("Exchange", exchangeName, sizeof(exchangeName));

                    static char apiKey[256] = "";
                    ImGui::InputText("API Key", apiKey, sizeof(apiKey), ImGuiInputTextFlags_Password);
                }
                else if (selectedNode->type == "Moving Average")
                {
                    static int period = 20;
                    ImGui::InputInt("Period", &period);

                    static int maType = 0;
                    ImGui::Combo("Type", &maType, "Simple\0Exponential\0Weighted\0");
                }
                // Add more node-specific properties as needed
            }
        }
        else
        {
            ImGui::Text("No node selected");
            ImGui::Spacing();
            ImGui::Text("Click on a node to edit its properties");
        }
    }
    ImGui::EndChild();
}

void ExchangeEditor::CreateNode(const std::string& type, const ImVec2& position)
{
    Node node;
    node.id = m_nextId++;
    node.type = type;
    node.name = type + " " + std::to_string(node.id);
    node.position = position;

    // TODO: Create pins based on node type
    // For now, just add basic input/output pins
    Pin inputPin;
    inputPin.id = m_nextId++;
    inputPin.nodeId = node.id;
    inputPin.name = "Input";
    inputPin.type = "Data";
    inputPin.isInput = true;

    Pin outputPin;
    outputPin.id = m_nextId++;
    outputPin.nodeId = node.id;
    outputPin.name = "Output";
    outputPin.type = "Data";
    outputPin.isInput = false;

    node.inputPins.push_back(inputPin.id);
    node.outputPins.push_back(outputPin.id);

    m_pins.push_back(inputPin);
    m_pins.push_back(outputPin);
    m_nodes.push_back(node);

    printf("Created node: %s (ID: %d)\n", node.name.c_str(), node.id);
}

void ExchangeEditor::DeleteNode(int nodeId)
{
    // Remove node
    m_nodes.erase(std::remove_if(m_nodes.begin(), m_nodes.end(),
                                [nodeId](const Node& node) { return node.id == nodeId; }),
                  m_nodes.end());

    // Remove associated pins
    m_pins.erase(std::remove_if(m_pins.begin(), m_pins.end(),
                               [nodeId](const Pin& pin) { return pin.nodeId == nodeId; }),
                 m_pins.end());

    // Remove associated links
    m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                                [this](const Link& link) {
                                    // Check if either pin belongs to deleted node
                                    for (const auto& pin : m_pins) {
                                        if (pin.id == link.startPinId || pin.id == link.endPinId)
                                            return false;
                                    }
                                    return true;
                                }),
                  m_links.end());

    if (m_selectedNodeId == nodeId)
        m_selectedNodeId = -1;

    printf("Deleted node ID: %d\n", nodeId);
}

void ExchangeEditor::CreateLink(int startPinId, int endPinId)
{
    Link link;
    link.id = m_nextId++;
    link.startPinId = startPinId;
    link.endPinId = endPinId;

    m_links.push_back(link);

    printf("Created link: %d -> %d\n", startPinId, endPinId);
}

void ExchangeEditor::DeleteLink(int linkId)
{
    m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                                [linkId](const Link& link) { return link.id == linkId; }),
                  m_links.end());

    printf("Deleted link ID: %d\n", linkId);
}

void ExchangeEditor::SaveConfiguration(const std::string& filename)
{
    // TODO: Implement JSON serialization
    printf("Saving configuration to: %s\n", filename.c_str());

    // Placeholder implementation
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << "{\n";
        file << "  \"nodes\": " << m_nodes.size() << ",\n";
        file << "  \"links\": " << m_links.size() << "\n";
        file << "}\n";
        file.close();
        printf("Configuration saved successfully\n");
    }
    else
    {
        printf("Failed to save configuration\n");
    }
}

void ExchangeEditor::LoadConfiguration(const std::string& filename)
{
    // TODO: Implement JSON deserialization
    printf("Loading configuration from: %s\n", filename.c_str());

    // Placeholder implementation
    std::ifstream file(filename);
    if (file.is_open())
    {
        // Clear current state
        m_nodes.clear();
        m_pins.clear();
        m_links.clear();
        m_nextId = 1;

        file.close();
        printf("Configuration loaded successfully\n");
    }
    else
    {
        printf("Failed to load configuration\n");
    }
}

void ExchangeEditor::Shutdown()
{
    // TODO: Cleanup node editor context
    // if (m_editorContext)
    // {
    //     ax::NodeEditor::DestroyEditor(m_editorContext.get());
    //     m_editorContext = nullptr;
    // }

    printf("ExchangeEditor shutdown\n");
}
} // namespace window
} // namespace gui