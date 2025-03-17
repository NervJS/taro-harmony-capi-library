//
// Created on 2024/7/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroListItemNode : public TaroRenderNode {
        public:
        TaroListItemNode(const TaroElementRef element);
        ~TaroListItemNode() override;

        void Build() override;
        void LayoutSelf();
    };

} // namespace TaroDOM
} // namespace TaroRuntime