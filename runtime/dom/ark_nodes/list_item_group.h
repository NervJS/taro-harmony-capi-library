//
// Created on 2024/8/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroListItemGroupNode : public TaroRenderNode {
        public:
        TaroListItemGroupNode(const TaroElementRef element);
        ~TaroListItemGroupNode() override;

        void Build() override;
        void LayoutSelf();

        void setHeader(ArkUI_NodeHandle handle);
        void setFooter(ArkUI_NodeHandle handle);
        void setSpace(float space);
        void Layout() override;
    };

} // namespace TaroDOM
} // namespace TaroRuntime
