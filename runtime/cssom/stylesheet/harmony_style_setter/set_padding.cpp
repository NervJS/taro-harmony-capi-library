/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
void HarmonyStyleSetter::setPadding(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto left = stylesheet->paddingLeft;
    auto right = stylesheet->paddingRight;
    auto top = stylesheet->paddingTop;
    auto bottom = stylesheet->paddingBottom;

    setFloat(node, NODE_PADDING, top, right, bottom, left);
};
void HarmonyStyleSetter::setPadding(const ArkUI_NodeHandle& node,
                                    const double& top,
                                    const double& right,
                                    const double& bottom,
                                    const double& left) {
    setFloat(node, NODE_PADDING, top, right, bottom, left);
}
void HarmonyStyleSetter::setPadding(
    const ArkUI_NodeHandle& node,
    const Optional<TaroRuntime::Dimension>&
        top,
    const Optional<TaroRuntime::Dimension>&
        right,
    const Optional<TaroRuntime::Dimension>&
        bottom,
    const Optional<TaroRuntime::Dimension>&
        left) {
    setFloat(node, NODE_PADDING, top, right, bottom, left);
}
void HarmonyStyleSetter::setPadding(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_PADDING);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet