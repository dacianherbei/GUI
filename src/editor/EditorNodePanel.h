#pragma once

#include "Node.h"
#include "imgui.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

// Forward declarations for imgui-node-editor
namespace ax { namespace NodeEditor { 
    struct EditorContext;
    using EditorContextPtr = std::unique_ptr<EditorContext, void(*)(EditorContext*)>;
    using LinkId = uintptr_t;
}}

namespace gui::editor
{
    struct Link
    {
        ax::NodeEditor::LinkId id;
        ax::NodeEditor::PinId startPinId;
        ax::NodeEditor::PinId endPinId;
        ImVec4 color;
        
        Link(ax::NodeEditor::LinkId linkId, ax::NodeEditor::PinId start, ax::NodeEditor::PinId end)
            : id(linkId), startPinId(start), endPinId(end), color(ImVec4(1,1,1,1)) {}
    };

    class EditorNodePanel
    {
    public:
        // Callback types
        using NodeCreateCallback = std::function<void(const std::string& nodeType, const ImVec2& position)>;
        using NodeDeleteCallback = std::function<void(ax::NodeEditor::NodeId nodeId)>;
        using NodeSelectCallback = std::function<void(ax::NodeEditor::NodeId nodeId)>;

        EditorNodePanel();
        ~EditorNodePanel();

        bool Initialize();
        void Render();
        void Shutdown();

        // Node management
        void AddNode(std::unique_ptr<Node> node);
        void RemoveNode(ax::NodeEditor::NodeId nodeId);
        Node* FindNode(ax::NodeEditor::NodeId nodeId);
        std::vector<Node*> GetSelectedNodes();
        void ClearSelection();

        // Link management
        void AddLink(ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId);
        void RemoveLink(ax::NodeEditor::LinkId linkId);
        Link* FindLink(ax::NodeEditor::LinkId linkId);
        std::vector<Link*> GetLinksForPin(ax::NodeEditor::PinId pinId);

        // Callbacks
        void SetNodeCreateCallback(NodeCreateCallback callback) { m_nodeCreateCallback = callback; }
        void SetNodeDeleteCallback(NodeDeleteCallback callback) { m_nodeDeleteCallback = callback; }
        void SetNodeSelectCallback(NodeSelectCallback callback) { m_nodeSelectCallback = callback; }

        // Editor state
        void ZoomToFit();
        void ZoomToContent();
        void CenterView();
        
        // Context menu
        void ShowContextMenu(const ImVec2& position);
        
        // Validation
        bool ValidateGraph();
        std::vector<std::string> GetValidationErrors();

        // Serialization
        void SaveToFile(const std::string& filename);
        void LoadFromFile(const std::string& filename);

    private:
        void HandleNodeEditor();
        void HandleLinkCreation();
        void HandleLinkDeletion();
        void HandleNodeSelection();
        void HandleNodeDeletion();
        void HandleContextMenu();
        
        void UpdateNodeConnections();
        void ValidateConnections();
        
        bool CanCreateLink(ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId);
        void UpdateLinkColors();

        // Node editor context
        ax::NodeEditor::EditorContextPtr m_editorContext;

        // Graph data
        std::vector<std::unique_ptr<Node>> m_nodes;
        std::vector<Link> m_links;
        std::unordered_map<ax::NodeEditor::NodeId, Node*> m_nodeMap;

        // Editor state
        ax::NodeEditor::NodeId m_selectedNodeId;
        std::vector<ax::NodeEditor::NodeId> m_selectedNodes;
        ImVec2 m_contextMenuPosition;
        bool m_showContextMenu;

        // Callbacks
        NodeCreateCallback m_nodeCreateCallback;
        NodeDeleteCallback m_nodeDeleteCallback;
        NodeSelectCallback m_nodeSelectCallback;

        // Static ID generators
        static ax::NodeEditor::NodeId GetNextNodeId();
        static ax::NodeEditor::LinkId GetNextLinkId();

        static ax::NodeEditor::NodeId s_nextNodeId;
        static ax::NodeEditor::LinkId s_nextLinkId;

        // Available node types for context menu
        std::vector<std::string> m_availableNodeTypes;
    };
}