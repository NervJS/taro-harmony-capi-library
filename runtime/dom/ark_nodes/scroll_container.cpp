/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./scroll_container.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroScrollContainerNode::TaroScrollContainerNode(TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroScrollContainerNode::~TaroScrollContainerNode() {}

    void TaroScrollContainerNode::setScrollEnableScrollInteraction(bool enable_value) {
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = enable_value;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
    }

    void TaroScrollContainerNode::setScrollBarDisplay(const ArkUI_ScrollBarDisplayMode& mode) {
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = mode;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_SCROLL_BAR_DISPLAY_MODE, &item);
    }

    void TaroScrollContainerNode::setScrollDirection(const ArkUI_ScrollDirection& direction) {
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = direction;
        NativeNodeApi::getInstance()->setAttribute(
            GetArkUINodeHandle(), NODE_SCROLL_SCROLL_DIRECTION, &item);
        direction_ = direction;
    }

    void TaroScrollContainerNode::setEdgeEffect(ArkUI_EdgeEffect edgeEffect) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[2] = {0, 0};
        ArkUI_AttributeItem item = {value, 2};
        value[0].i32 = edgeEffect;
        value[1].i32 = 0;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SCROLL_EDGE_EFFECT, &item);
    };

    void TaroScrollContainerNode::SetScrollOffset(const double& scrollLeft, const double& scrollTop, bool enableAnimation, int32_t duration) {
        ArkUI_NumberValue value[6] = {0};
        ArkUI_AttributeItem item = {value, 6};
        value[0].f32 = scrollLeft;
        value[1].f32 = scrollTop;
        value[2].i32 = enableAnimation ? duration : 0;
        value[4].i32 = ARKUI_CURVE_LINEAR;

        NativeNodeApi::getInstance()->setAttribute(
            GetArkUINodeHandle(), NODE_SCROLL_OFFSET, &item);
    }

    void TaroScrollContainerNode::setNestedMode(ArkUI_ScrollNestedMode downMode, ArkUI_ScrollNestedMode upMode) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[2] = {0, 0};
        value[0].i32 = downMode;
        value[1].i32 = upMode;
        ArkUI_AttributeItem item{value, 2};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SCROLL_NESTED_SCROLL, &item);
    }

    void TaroScrollContainerNode::setFriction(double friction) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[1] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].f32 = friction;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SCROLL_FRICTION, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
