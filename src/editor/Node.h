#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
namespace ax { namespace NodeEditor { 
    using PinId = uintptr_t;
    using NodeId = uintptr_t; 
}}

namespace gui::editor
{
class Node {
public:
    Node(ax::NodeEditor::NodeId nodeId, const std::string& nodeType, const std::string& title);
    virtual ~Node() = default;

    // Core interface
    virtual void Render() = 0;
    virtual void Update(float deltaTime) {}
    virtual void OnInputConnected(ax::NodeEditor::PinId pinId) {}
    virtual void OnInputDisconnected(ax::NodeEditor::PinId pinId) {}
    virtual void OnOutputConnected(ax::NodeEditor::PinId pinId) {}
    virtual void OnOutputDisconnected(ax::NodeEditor::PinId pinId) {}

    // Data type validation - override in derived classes
    virtual bool CanAcceptInput(ax::NodeEditor::PinId inputPin, const NodeData* outputData) const {
        return false; // Base class doesn't accept any input by default
    }

    virtual std::shared_ptr<NodeData> GetOutputData(ax::NodeEditor::PinId outputPin) const {
        return nullptr; // Base class doesn't output data by default
    }

    // Getters
    ax::NodeEditor::NodeId GetId() const { return m_id; }
    const std::string& GetType() const { return m_type; }
    const std::string& GetTitle() const { return m_title; }
    const std::vector<Pin>& GetInputPins() const { return m_inputPins; }
    const std::vector<Pin>& GetOutputPins() const { return m_outputPins; }
    ImVec2 GetPosition() const { return m_position; }
    ImVec2 GetSize() const { return m_size; }

    // Setters
    void SetTitle(const std::string& title) { m_title = title; }
    void SetPosition(const ImVec2& position) { m_position = position; }
    void SetSize(const ImVec2& size) { m_size = size; }

    // Pin management
    Pin* FindPin(ax::NodeEditor::PinId pinId);
    Pin* FindInputPin(const std::string& name);
    Pin* FindOutputPin(const std::string& name);

    // Data flow methods
    void SetInputData(ax::NodeEditor::PinId pinId, std::shared_ptr<NodeData> data);
    std::shared_ptr<NodeData> GetInputData(ax::NodeEditor::PinId pinId) const;
    void SetOutputData(ax::NodeEditor::PinId pinId, std::shared_ptr<NodeData> data);

    // Connection validation
    virtual bool CanConnect(ax::NodeEditor::PinId outputPin, ax::NodeEditor::PinId inputPin) const;
    virtual bool IsValid() const { return true; }

    // Serialization
    virtual void Serialize(std::ostream& out) const;
    virtual void Deserialize(std::istream& in);

    // Node creation interface
    virtual std::unique_ptr<Node> Clone() const = 0;

protected:
    // Helper methods for derived classes
    void AddInputPin(const std::string& name, DataType dataType,
                    ImVec4 color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    void AddOutputPin(const std::string& name, DataType dataType,
                     ImVec4 color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

    void RenderPinIcon(const Pin& pin, bool connected);
    void BeginNode();
    void EndNode();

    // Static ID generator
    static ax::NodeEditor::PinId GetNextPinId();

private:
    ax::NodeEditor::NodeId m_id;
    std::string m_type;
    std::string m_title;
    ImVec2 m_position;
    ImVec2 m_size;

    std::vector<Pin> m_inputPins;
    std::vector<Pin> m_outputPins;

    static ax::NodeEditor::PinId s_nextPinId;
};

    // Color constants for different data types
    namespace Colors
    {
        constexpr ImVec4 EndPoint       = ImVec4(0.9f, 0.6f, 0.2f, 1.0f); // Orange
        constexpr ImVec4 Connection     = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); // Green
        constexpr ImVec4 AccountConfig  = ImVec4(0.2f, 0.6f, 0.9f, 1.0f); // Blue
        constexpr ImVec4 AccountList    = ImVec4(0.4f, 0.4f, 0.9f, 1.0f); // Dark Blue
        constexpr ImVec4 MessageStream  = ImVec4(0.9f, 0.2f, 0.6f, 1.0f); // Pink
        constexpr ImVec4 Boolean        = ImVec4(0.8f, 0.8f, 0.2f, 1.0f); // Yellow
        constexpr ImVec4 String         = ImVec4(0.6f, 0.9f, 0.6f, 1.0f); // Light Green
        constexpr ImVec4 Integer        = ImVec4(0.9f, 0.5f, 0.9f, 1.0f); // Magenta
    }

    // Utility function to get color for data type
    ImVec4 GetColorForDataType(DataType type);
}