/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBackgroundSize(
    const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet, const TaroDOM::ComputedStyle& computed_style) {
    auto backgroundSize = stylesheet->backgroundSize;
    auto width = computed_style.width;
    auto height = computed_style.height;
    setBackgroundSize(node, backgroundSize, width, height);
}
void HarmonyStyleSetter::setBackgroundSize(
    const ArkUI_NodeHandle& node, const BackgroundSizeParam& value, const float& width, const float& height) {
    if (value.type == IMAGESIZE && value.imageSize.has_value()) {
        ArkUI_NumberValue arkUI_NumberValue[1] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        arkUI_NumberValue[0].i32 = value.imageSize.value();
        TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
            node, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE, &item);
    } else if (value.type == SIZEOPTIONS) {
        if (!value.width.has_value() || !value.height.has_value())
            return;
        auto harmonyWidth = value.width.value().ParseToVp(width);
        auto harmonyHeight = value.height.value().ParseToVp(height);
        if (!harmonyWidth.has_value() || !harmonyHeight.has_value())
            return;

        ArkUI_NumberValue arkUI_NumberValue[2] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 2};

        arkUI_NumberValue[0].f32 = harmonyWidth.value();
        arkUI_NumberValue[1].f32 = harmonyHeight.value();

        TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, NODE_BACKGROUND_IMAGE_SIZE, &item);
    }
}
void HarmonyStyleSetter::setBackgroundSize(const ArkUI_NodeHandle& node, const Optional<BackgroundSizeParam>& value, const float& width, const float& height) {
    if (value.has_value()) {
        setBackgroundSize(node, value.value(), width, height);
    }
}
void HarmonyStyleSetter::setBackgroundSize(const ArkUI_NodeHandle& node) {
    TaroRuntime::NativeNodeApi::getInstance()->resetAttribute(node, NODE_BACKGROUND_IMAGE_SIZE);
    TaroRuntime::NativeNodeApi::getInstance()->resetAttribute(node, NODE_BACKGROUND_IMAGE_SIZE_WITH_STYLE);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
