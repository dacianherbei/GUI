#pragma once

#include "Node.h"

namespace gui::editor
{
struct EndPointConfiguration
{
    std::string webAddress;
    int port;
    bool useSSL;
    std::string protocol; // "WebSocket", "HTTP", "UDP", "FIX"

    EndPointConfiguration()
        : port(443), useSSL(true), protocol("WebSocket") {}
};

class EndPointNode : public Node
{
public:
    EndPointNode(ax::NodeEditor::NodeId nodeId);
    virtual ~EndPointNode() = default;

    // Node interface
    void Render() override;
    void Update(float deltaTime) override;
    bool IsValid() const override;

    // Configuration
    const EndPointConfiguration& GetConfiguration() const { return m_config; }
    void SetConfiguration(const EndPointConfiguration& config) { m_config = config; }

    // Serialization
    void Serialize(std::ostream& out) const override;
    void Deserialize(std::istream& in) override;

private:
    void RenderConfigurationUI();
    void UpdateOutputData();
    void ValidateConfiguration();

    EndPointConfiguration m_config;

    // UI state
    char m_webAddressBuffer[256];
    bool m_configExpanded;

    // Validation state
    bool m_isConfigValid;
    std::string m_validationError;

    // Common endpoints for quick selection
    struct PresetEndpoint
    {
        std::string name;
        std::string address;
        int port;
        bool useSSL;
        std::string protocol;
    };

    static std::vector<PresetEndpoint> GetPresetEndpoints();
    void ApplyPreset(const PresetEndpoint& preset);
};
}