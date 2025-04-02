/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
