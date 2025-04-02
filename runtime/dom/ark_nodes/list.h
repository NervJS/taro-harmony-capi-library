/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "arkui_node.h"
#include "arkui_node_adapter.h"
#include "runtime/dom/ark_nodes/scroll_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroListNode : public TaroScrollContainerNode {
        public:
        TaroListNode(const TaroElementRef element);
        ~TaroListNode() override;

        void Build() override;

        void setStickyStyle(ArkUI_StickyStyle style);

        // 首次布局时会挂载 adapter
        void Layout() override;

        void setCacheCount(int32_t count);
        void setSpace(float space);

        std::shared_ptr<TaroNodeAdapter> adapter_;

        private:
        bool init_adapter_;
        void LayoutSelf();
    };

} // namespace TaroDOM
} // namespace TaroRuntime