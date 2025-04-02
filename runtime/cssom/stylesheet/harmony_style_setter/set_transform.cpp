/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "folly/lang/Exception.h"
#include "harmony_style_setter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {
    auto transform = stylesheet->transform;
    if (transform.has_value()) {
        setTransform(node, transform.value());
    }
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const TransformParam& value) {
    setTransform(node, TransformParam::staticTransformItemsToMatrix4(value).toVector());
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const TransformParam& value, float widthBase, float heightBase) {
    setTransform(node, TransformParam::staticTransformItemsToMatrix4(value, widthBase, heightBase).toVector());
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const Optional<TransformParam>& value) {
    if (value.has_value()) {
        setTransform(node, TransformParam::staticTransformItemsToMatrix4(value.value()).toVector());
    }
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const Optional<TransformParam>& value, float widthBase, float heightBase) {
    if (value.has_value()) {
        setTransform(node, TransformParam::staticTransformItemsToMatrix4(value.value(), widthBase, heightBase).toVector());
    }
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node, const std::vector<float>& value) {
    if (value.size() != 16)
        return;

    ArkUI_NumberValue arkUI_NumberValue[16] = {};
    std::transform(value.begin(), value.end(), arkUI_NumberValue, [](float v) {
        ArkUI_NumberValue num;
        num.f32 = v;
        return num;
    });

    ArkUI_AttributeItem item = {arkUI_NumberValue, 16};
    std::stringstream ss;
    for (int idx = 0; idx < item.size; idx++) {
#if IS_DEBUG
        if (std::isnan(item.value[idx].f32)) {
            folly::throw_exception(std::runtime_error("invalid float"));
        }
#endif
        ss << item.value[idx].f32 << " ";
    }
    TARO_LOG_DEBUG("Transform", "transform:%{public}s", ss.str().c_str());
    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, NODE_TRANSFORM,
                                                            &item);
}
void HarmonyStyleSetter::setTransform(const ArkUI_NodeHandle& node) {
    TaroRuntime::NativeNodeApi::getInstance()->resetAttribute(node, NODE_TRANSFORM);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
