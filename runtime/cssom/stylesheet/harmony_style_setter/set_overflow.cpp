//
// Created on 2024/6/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
void HarmonyStyleSetter::setOverflow(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {
    auto overflow = stylesheet->overflow;
    setOverflow(node, overflow);
}
void HarmonyStyleSetter::setOverflow(const ArkUI_NodeHandle& node,
                                     const int& val) {
    setInt(node, NODE_CLIP, val);
}
void HarmonyStyleSetter::setOverflow(const ArkUI_NodeHandle& node,
                                     const Optional<PropertyType::Overflow>& val) {
    if (val.has_value()) {
        switch (val.value()) {
            case PropertyType::Overflow::Visible:
                setInt(node, NODE_CLIP, 0);
                break;
            default:
                setInt(node, NODE_CLIP, 1);
        }
    }
}
void HarmonyStyleSetter::setOverflow(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_CLIP);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet