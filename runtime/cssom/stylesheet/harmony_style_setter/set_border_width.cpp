/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setBorderWidth(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto left = stylesheet->borderLeftWidth;
    auto right = stylesheet->borderRightWidth;
    auto top = stylesheet->borderTopWidth;
    auto bottom = stylesheet->borderBottomWidth;

    setFloat(node, NODE_BORDER_WIDTH, top, right, bottom, left);
};
void HarmonyStyleSetter::setBorderWidth(const ArkUI_NodeHandle& node,
                                        const double& top,
                                        const double& right,
                                        const double& bottom,
                                        const double& left) {
    setFloat(node, NODE_BORDER_WIDTH, top, right, bottom, left);
}
void HarmonyStyleSetter::setBorderWidth(
    const ArkUI_NodeHandle& node,
    const Optional<Dimension>& top,
    const Optional<Dimension>& right,
    const Optional<Dimension>& bottom,
    const Optional<Dimension>& left) {
    setFloat(node, NODE_BORDER_WIDTH, top, right, bottom, left);
}
void HarmonyStyleSetter::setBorderWidth(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BORDER_WIDTH);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet