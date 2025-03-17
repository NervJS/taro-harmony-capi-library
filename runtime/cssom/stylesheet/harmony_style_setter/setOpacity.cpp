//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setOpacity(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto opacity = stylesheet->opacity;
    if (opacity.has_value()) {
        setOpacity(node, opacity);
    }
}
void HarmonyStyleSetter::setOpacity(const ArkUI_NodeHandle& node,
                                    const double& val) {
    setFloat(node, NODE_OPACITY, val);
}
void HarmonyStyleSetter::setOpacity(const ArkUI_NodeHandle& node,
                                    const Optional<float>& val) {
    setFloat(node, NODE_OPACITY, val);
}
void HarmonyStyleSetter::setOpacity(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_OPACITY);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet