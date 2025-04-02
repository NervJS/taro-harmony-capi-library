/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBorderColor(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto left = stylesheet->borderLeftColor;
    auto right = stylesheet->borderRightColor;
    auto top = stylesheet->borderTopColor;
    auto bottom = stylesheet->borderBottomColor;

    setBorderColor(node, top, right, bottom, left);
}
void HarmonyStyleSetter::setBorderColor(const ArkUI_NodeHandle& node,
                                        const uint32_t& top,
                                        const uint32_t right,
                                        const uint32_t& bottom,
                                        const uint32_t& left) {
    ArkUI_NumberValue arkUI_NumberValue[4] = {};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 4};

    arkUI_NumberValue[0].u32 = top;
    arkUI_NumberValue[1].u32 = right;
    arkUI_NumberValue[2].u32 = bottom;
    arkUI_NumberValue[3].u32 = left;

    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
        node, NODE_BORDER_COLOR, &item);
}
void HarmonyStyleSetter::setBorderColor(
    const ArkUI_NodeHandle& node, const Optional<uint32_t>& top,
    const Optional<uint32_t>& right, const Optional<uint32_t>& bottom,
    const Optional<uint32_t>& left) {
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
        node, NODE_BORDER_COLOR, &item);
}
void HarmonyStyleSetter::setBorderColor(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BORDER_COLOR);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
