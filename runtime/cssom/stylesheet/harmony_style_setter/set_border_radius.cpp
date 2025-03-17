//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBorderRadius(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {
    auto topLeft = stylesheet->borderTopLeftRadius;
    auto topRight = stylesheet->borderTopRightRadius;
    auto bottomLeft = stylesheet->borderBottomLeftRadius;
    auto bottomRight = stylesheet->borderBottomRightRadius;

    setFloat(node, NODE_BORDER_RADIUS, topLeft, topRight, bottomLeft, bottomRight);
}
void HarmonyStyleSetter::setBorderRadius(
    const ArkUI_NodeHandle& node,
    double topLeft,
    double topRight,
    double bottomRight,
    double bottomLeft) {
    setFloat(node, NODE_BORDER_RADIUS, topLeft, topRight, bottomLeft, bottomRight);
}

void HarmonyStyleSetter::setBorderRadius(
    const ArkUI_NodeHandle& node,
    const Optional<Dimension>& topLeft,
    const Optional<Dimension>& topRight,
    const Optional<Dimension>& bottomRight,
    const Optional<Dimension>& bottomLeft) {
    setFloat(node, NODE_BORDER_RADIUS, topLeft, topRight, bottomLeft, bottomRight);
}

void HarmonyStyleSetter::setBorderRadius(
    const ArkUI_NodeHandle& node,
    const Optional<Dimension>& topLeft,
    const Optional<Dimension>& topRight,
    const Optional<Dimension>& bottomRight,
    const Optional<Dimension>& bottomLeft,
    float baseValue) {
    setFloat(node, NODE_BORDER_RADIUS, topLeft, topRight, bottomLeft, bottomRight, baseValue);
}

void HarmonyStyleSetter::setBorderRadius(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BORDER_RADIUS);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet