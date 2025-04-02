/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setTransformOrigin(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {
    setTransformOrigin(node, stylesheet->transformOrigin);
}

void HarmonyStyleSetter::setTransformOrigin(const ArkUI_NodeHandle& node,
                                            const TransformOriginData& value) {
    ArkUI_NumberValue arkUI_NumberValue[6] = {};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 6};

    if (auto val = value.x.ParseToVp(); val.has_value())
        arkUI_NumberValue[0].f32 = val.value();
    if (auto val = value.y.ParseToVp(); val.has_value())
        arkUI_NumberValue[1].f32 = val.value();
    if (auto val = value.z.ParseToVp(); val.has_value())
        arkUI_NumberValue[2].f32 = val.value();
    if (value.px.IsValid())
        arkUI_NumberValue[3].f32 = value.px.Value();
    if (value.py.IsValid())
        arkUI_NumberValue[4].f32 = value.py.Value();
    if (value.pz.IsValid())
        arkUI_NumberValue[5].f32 = value.pz.Value();

    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, NODE_TRANSFORM_CENTER, &item);
}
void HarmonyStyleSetter::setTransformOrigin(const ArkUI_NodeHandle& node,
                                            const Optional<TransformOriginData>& val) {
    if (val.has_value()) {
        setTransformOrigin(node, val.value());
    }
}
void HarmonyStyleSetter::setTransformOrigin(const ArkUI_NodeHandle& node) {
    TaroRuntime::NativeNodeApi::getInstance()->resetAttribute(
        node, NODE_TRANSFORM_CENTER);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
