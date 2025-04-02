/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_SCROLL_NODE_CONTAINER_H
#define TARO_HARMONY_CPP_SCROLL_NODE_CONTAINER_H

#include "arkui/native_type.h"
#include "arkui_node.h"
#include "runtime/dom/ark_nodes/scroll_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroScrollContainerNode : public TaroRenderNode {
        public:
        TaroScrollContainerNode(const TaroElementRef element);

        virtual ~TaroScrollContainerNode();

        void setScrollEnableScrollInteraction(bool enable_value);

        void setScrollBarDisplay(const ArkUI_ScrollBarDisplayMode& mode);

        void setScrollDirection(const ArkUI_ScrollDirection& direction);

        void SetScrollOffset(const double& scrollLeft, const double& scrollTop, bool enableAnimation, int32_t duration = 500);

        void setNestedMode(ArkUI_ScrollNestedMode downMode, ArkUI_ScrollNestedMode upMode);

        void setEdgeEffect(ArkUI_EdgeEffect edgeEffect);

        void setFriction(double friction);

        virtual void Build() = 0;

        ArkUI_ScrollDirection direction_ = ARKUI_SCROLL_DIRECTION_VERTICAL;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_SCROLL_NODE_CONTAINER_H
