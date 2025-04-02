/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
