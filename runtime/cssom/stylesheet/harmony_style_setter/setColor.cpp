//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setColor(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto color = stylesheet->color;
    setColor(node, color);
}
void HarmonyStyleSetter::setColor(const ArkUI_NodeHandle& node,
                                  const uint32_t& val) {
    setUint(node, NODE_FONT_COLOR, val);
}
void HarmonyStyleSetter::setColor(const ArkUI_NodeHandle& node,
                                  const Optional<uint32_t>& val) {
    setUint(node, NODE_FONT_COLOR, val);
}
void HarmonyStyleSetter::setColor(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_FONT_COLOR);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet