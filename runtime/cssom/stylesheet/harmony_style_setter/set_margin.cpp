//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setMargin(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto left = stylesheet->borderLeftWidth;
    auto right = stylesheet->borderRightWidth;
    auto top = stylesheet->borderTopWidth;
    auto bottom = stylesheet->borderBottomWidth;

    setFloat(node, NODE_MARGIN, top, right, bottom, left);
};
void HarmonyStyleSetter::setMargin(const ArkUI_NodeHandle& node,
                                   const double& top,
                                   const double& right,
                                   const double& bottom,
                                   const double& left) {
    setFloat(node, NODE_MARGIN, top, right, bottom, left);
}
void HarmonyStyleSetter::setMargin(
    const ArkUI_NodeHandle& node,
    const Optional<TaroRuntime::Dimension>& top,
    const Optional<TaroRuntime::Dimension>& right,
    const Optional<TaroRuntime::Dimension>& bottom,
    const Optional<TaroRuntime::Dimension>& left) {
    setFloat(node, NODE_MARGIN, top, right, bottom, left);
}
void HarmonyStyleSetter::setMargin(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_MARGIN);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet