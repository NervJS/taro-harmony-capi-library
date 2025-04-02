/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "progress.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroProgressNode::TaroProgressNode(const TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroProgressNode::~TaroProgressNode() {}

    void TaroProgressNode::Build() {
        SetArkUINodeHandle(NativeNodeApi::getInstance()->createNode(ARKUI_NODE_PROGRESS));
    }

    void TaroProgressNode::SetPercent(float percent) {
        ArkUI_NumberValue value[] = {{.f32 = percent}};
        ArkUI_AttributeItem attr = {value, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_PROGRESS_VALUE, &attr);
    }

    void TaroProgressNode::SetType(ArkUI_ProgressType type) {
        ArkUI_NumberValue value[] = {{.u32 = type}};
        ArkUI_AttributeItem attr = {value, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_PROGRESS_TYPE, &attr);
    }

    void TaroProgressNode::SetStrokeWidth(uint32_t value) {
        TARO_LOG_WARN("TaroProgressNode", "SetStrokeWidth %{public}d fail: 鸿蒙 CAPI 暂不支持 strokeWidth 属性", value);
    }

    void TaroProgressNode::SetStrokeRadius(uint32_t value) {
        TARO_LOG_WARN("TaroProgressNode", "SetStrokeRadius %{public}d fail: 鸿蒙 CAPI 暂不支持 strokeRadius 属性", value);
    }

    void TaroProgressNode::SetActiveColor(uint32_t value) {
        ArkUI_NumberValue color[] = {{.u32 = value}};
        ArkUI_AttributeItem colorItem = {color, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_PROGRESS_COLOR, &colorItem);
    }

    void TaroProgressNode::SetBackgroundColor(uint32_t value) {
        TARO_LOG_WARN("TaroProgressNode", "SetBackgroundColor %{public}d fail: 鸿蒙 CAPI 暂不支持 backgroundColor 属性", value);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
