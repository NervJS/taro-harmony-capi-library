//
// Created on 2024/6/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBoxShadow(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto boxShadow = stylesheet->boxShadow;
    setBoxShadow(node, boxShadow);
}
void HarmonyStyleSetter::setBoxShadow(const ArkUI_NodeHandle& node, const BoxShadowItem& value) {
    ArkUI_NumberValue arkUI_NumberValue[] = {0.0, 0, 0.0, 0.0, 0, 0, 0};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 7};

    if (value.radius.has_value()) {
        arkUI_NumberValue[0].f32 = vp2Px(value.radius.value().ParseToVp().value_or(0.0f)); // 怀疑是文档的问题，这里实际上要设置像素值才能达到效果
    }

    if (value.offsetX.has_value()) {
        arkUI_NumberValue[2].f32 = vp2Px(value.offsetX.value().ParseToVp().value_or(0.0f));
    }
    if (value.offsetY.has_value()) {
        arkUI_NumberValue[3].f32 =vp2Px(value.offsetY.value().ParseToVp().value_or(0.0f));
    }

    if (value.color.has_value()) {
        auto color = value.color.value();
        arkUI_NumberValue[5].u32 = color;
    }
    if (value.fill.has_value()) {
        auto fill = value.fill.value();
        arkUI_NumberValue[6].u32 = fill;
    }

    NativeNodeApi::getInstance()->setAttribute(node, NODE_CUSTOM_SHADOW, &item);
}
void HarmonyStyleSetter::setBoxShadow(const ArkUI_NodeHandle& node,
                                      const Optional<BoxShadowItem>& val) {
    if (val.has_value()) {
        setBoxShadow(node, val.value());
    }
}
void HarmonyStyleSetter::setBoxShadow(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_CUSTOM_SHADOW);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet