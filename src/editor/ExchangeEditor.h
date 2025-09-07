#pragma once

#include "imgui.h"
#include "EditorNodePanel.h"
#include <string>
#include <memory>

namespace gui::editor
{
    class Node;
    class EndPointNode;
    class FixConnectionNode;
    class WebSocketConnectionNode;
    class UDPConnectionNode;
    class RestConnectionNode;
    class AccountConfigurationLoaderNode;
    class AccountSelectorNode;
}

class ExchangeEditor
{
public:
    enum class Action
    {
        None,
        Back,
        Save,
        Close
    };

    ExchangeEditor();
    ~ExchangeEditor();
    
    bool Initialize();
    void Render();
    void Shutdown();
    
    Action GetAction() const { return m_action; }
    void ResetAction() { m_action = Action::None; }
    
    bool ShouldShow() const { return m_shouldShow; }
    void Show() { m_shouldShow = true; }
    void Hide() { m_shouldShow = false; }
    
    // Configuration management
    void SaveConfiguration(const std::string& filename);
    void LoadConfiguration(const std::string& filename);
    void NewConfiguration();

private:
    void RenderMenuBar();
    void RenderMainLayout();
    void RenderNodePalette();
    void RenderPropertiesPanel();
    void RenderStatusBar();
    
    // Node creation callbacks
    void OnNodeCreateRequested(const std::string& nodeType, const ImVec2& position);
    void OnNodeDeleteRequested(ax::NodeEditor::NodeId nodeId);
    void OnNodeSelected(ax::NodeEditor::NodeId nodeId);
    
    // Node factory methods
    std::unique_ptr<gui::editor::Node> CreateNode(const std::string& nodeType, ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::EndPointNode> CreateEndPointNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::FixConnectionNode> CreateFixConnectionNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::WebSocketConnectionNode> CreateWebSocketConnectionNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::UDPConnectionNode> CreateUDPConnectionNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::RestConnectionNode> CreateRestConnectionNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::AccountConfigurationLoaderNode> CreateAccountLoaderNode(ax::NodeEditor::NodeId nodeId);
    std::unique_ptr<gui::editor::AccountSelectorNode> CreateAccountSelectorNode(ax::NodeEditor::NodeId nodeId);
    
    // UI state management
    void UpdateSelectedNodeProperties();
    void ValidateConfiguration();
    
    bool m_shouldShow;
    Action m_action;
    
    // Core editor component
    std::unique_ptr<gui::editor::EditorNodePanel> m_nodePanel;
    
    // UI panels
    bool m_showNodePalette;
    bool m_showPropertiesPanel;
    bool m_showStatusBar;
    
    // Selected node for properties panel
    ax::NodeEditor::NodeId m_selectedNodeId;
    gui::editor::Node* m_selectedNode;
    
    // Configuration state
    std::string m_currentFilePath;
    bool m_hasUnsavedChanges;
    
    // Node palette organization
    struct NodeCategory
    {
        std::string name;
        std::vector<std::string> nodeTypes;
        bool expanded;
    };
    
    std::vector<NodeCategory> m_nodeCategories;
    
    // Validation and errors
    std::vector<std::string> m_validationErrors;
    std::vector<std::string> m_validationWarnings;
    
    // Statistics
    struct EditorStats
    {
        int totalNodes;
        int totalConnections;
        int validationErrors;
        int validationWarnings;
        std::string lastSaveTime;
    };
    
    EditorStats m_stats;
    
    void UpdateStatistics();
    void InitializeNodeCategories();
};