//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroFlowItemNode : public TaroRenderNode {
        public:
        TaroFlowItemNode(const TaroElementRef element);
        ~TaroFlowItemNode() override;

        void Build() override;
        void Layout() override;
        void LayoutSelf();
    };

} // namespace TaroDOM
} // namespace TaroRuntime
