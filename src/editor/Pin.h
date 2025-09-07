#pragma once

#include "DataType.h"
#include "PinType.h"

#include <imgui_node_editor.h>

namespace gui
{
    namespace editor
    {
        struct Pin {
            ax::NodeEditor::PinId id;
            std::string name;
            DataType dataType;
            PinType pinType;
            ImVec4 color;
            bool isConnected;
            std::shared_ptr<NodeData> data;  // Actual data flowing through this pin

            Pin(ax::NodeEditor::PinId pinId, const std::string& pinName,
                DataType type, PinType pinKind, ImVec4 pinColor = ImVec4(1,1,1,1))
                : id(pinId), name(pinName), dataType(type), pinType(pinKind),
                  color(pinColor), isConnected(false) {}
        };
    } // editor
} // gui

#endif //TERMINAL_PIN_H