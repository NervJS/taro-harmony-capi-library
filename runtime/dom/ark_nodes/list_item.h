/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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