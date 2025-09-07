
#pragma once

#include "imgui.h"

#include <string>
#include <vector>
#include <memory>

// Forward declaration for imgui-node-editor
namespace ax { namespace NodeEditor {
struct EditorContext;
using EditorContextPtr = std::unique_ptr<EditorContext, void(*)(EditorContext*)>;
}}

namespace gui {
namespace window {

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

    struct Node
    {
        int id;
        std::string type;
        std::string name;
        ImVec2 position;
        std::vector<int> inputPins;
        std::vector<int> outputPins;
    };

    struct Pin
    {
        int id;
        int nodeId;
        std::string name;
        std::string type;
        bool isInput;
        ImVec2 position;
    };

    struct Link
    {
        int id;
        int startPinId;
        int endPinId;
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

private:
    void RenderMenuBar();
    void RenderNodeEditor();
    void RenderPropertiesPanel();
    void RenderNodePalette();

    void CreateNode(const std::string& type, const ImVec2& position);
    void DeleteNode(int nodeId);
    void CreateLink(int startPinId, int endPinId);
    void DeleteLink(int linkId);

    // Node editor context
    ax::NodeEditor::EditorContextPtr m_editorContext;

    bool m_shouldShow;
    Action m_action;

    // Editor data
    std::vector<Node> m_nodes;
    std::vector<Pin> m_pins;
    std::vector<Link> m_links;

    // UI state
    int m_nextId;
    int m_selectedNodeId;
    bool m_showPropertiesPanel;
    bool m_showNodePalette;

    // Available node types
    std::vector<std::string> m_nodeTypes;
};

} // namespace window
} // namespace gui
