/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBackgroundColor(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto backgroundColor = stylesheet->backgroundColor;
    setBackgroundColor(node, backgroundColor);
}
void HarmonyStyleSetter::setBackgroundColor(
    const ArkUI_NodeHandle& node, const uint32_t& val) {
    setUint(node, NODE_BACKGROUND_COLOR, val);
}
void HarmonyStyleSetter::setBackgroundColor(
    const ArkUI_NodeHandle& node, const Optional<uint32_t>& val) {
    setUint(node, NODE_BACKGROUND_COLOR, val);
}
void HarmonyStyleSetter::setBackgroundColor(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BACKGROUND_COLOR);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet