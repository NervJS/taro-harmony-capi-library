/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "checkbox.h"
namespace TaroRuntime {
namespace TaroDOM {
    TaroCheckboxNode::TaroCheckboxNode(const TaroElementRef element)
        : TaroRenderNode(element) {}
    TaroCheckboxNode::~TaroCheckboxNode() {}

    void TaroCheckboxNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_CHECKBOX));
        SetZeroMargin();
    }

    void TaroCheckboxNode::SetZeroMargin() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        numberValue->f32 = 0;
        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_MARGIN, &item);
    }

    void TaroCheckboxNode::ChangeChecked() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        const ArkUI_AttributeItem *item = nativeNodeApi->getAttribute(GetArkUINodeHandle(), NODE_CHECKBOX_SELECT);
        SetChecked(!item->value[0].i32);
    }

    void TaroCheckboxNode::SetChecked(bool checked) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        int32_t isChecked = checked ? 1 : 0;
        numberValue[0].i32 = isChecked;
        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_CHECKBOX_SELECT, &item);
    }

    void TaroCheckboxNode::SetColor(uint32_t color) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        numberValue[0].u32 = color;
        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_CHECKBOX_SELECT_COLOR, &item);
    }

    void TaroCheckboxNode::SetShape(std::string shape) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        if (shape == "circle") {
            numberValue[0].i32 = ArkUI_CHECKBOX_SHAPE_CIRCLE;
        } else {
            numberValue[0].i32 = ArkUI_CHECKBOX_SHAPE_ROUNDED_SQUARE;
        }

        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_CHECKBOX_SHAPE, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime