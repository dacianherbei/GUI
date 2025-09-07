#pragma once

#include <memory>

namespace gui {
    namespace editor {
        class NodeData {
        public:
            virtual ~NodeData() = default;
            virtual std::unique_ptr<NodeData> Clone() const = 0;
            virtual std::type_index GetTypeIndex() const = 0;

            template<typename T>
            T* As() {
                if (GetTypeIndex() == std::type_index(typeid(T))) {
                    return static_cast<T*>(this);
                }
                return nullptr;
            }

            template<typename T>
            const T* As() const {
                if (GetTypeIndex() == std::type_index(typeid(T))) {
                    return static_cast<const T*>(this);
                }
                return nullptr;
            }
        };
    }
}