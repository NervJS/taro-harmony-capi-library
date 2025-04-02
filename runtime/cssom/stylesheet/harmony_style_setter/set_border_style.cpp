/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBorderStyle(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto left = stylesheet->borderLeftStyle;
    auto right = stylesheet->borderRightStyle;
    auto top = stylesheet->borderTopStyle;
    auto bottom = stylesheet->borderBottomStyle;

    setBorderStyle(node, top, right, bottom, left);
}
void HarmonyStyleSetter::setBorderStyle(const ArkUI_NodeHandle& node,
                                        const ArkUI_BorderStyle& top,
                                        const ArkUI_BorderStyle right,
                                        const ArkUI_BorderStyle& bottom,
                                        const ArkUI_BorderStyle& left) {
    ArkUI_NumberValue arkUI_NumberValue[4] = {};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 4};

    arkUI_NumberValue[0].i32 = top;
    arkUI_NumberValue[1].i32 = right;
    arkUI_NumberValue[2].i32 = bottom;
    arkUI_NumberValue[3].i32 = left;

    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
        node, NODE_BORDER_STYLE, &item);
}
void HarmonyStyleSetter::setBorderStyle(
    const ArkUI_NodeHandle& node,
    const Optional<ArkUI_BorderStyle>& top,
    const Optional<ArkUI_BorderStyle>& right,
    const Optional<ArkUI_BorderStyle>& bottom,
    const Optional<ArkUI_BorderStyle>& left) {
    ArkUI_NumberValue arkUI_NumberValue[4] = {};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 4};

    if (top.has_value()) {
        arkUI_NumberValue[0].i32 = top.value();
    }
    if (right.has_value()) {
        arkUI_NumberValue[1].i32 = right.value();
    }
    if (bottom.has_value()) {
        arkUI_NumberValue[2].i32 = bottom.value();
    }
    if (left.has_value()) {
        arkUI_NumberValue[3].i32 = left.value();
    }

    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
        node, NODE_BORDER_STYLE, &item);
}
void HarmonyStyleSetter::setBorderStyle(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BORDER_STYLE);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
