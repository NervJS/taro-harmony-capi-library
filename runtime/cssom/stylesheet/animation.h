/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_ANIMATION_H
#define TARO_CAPI_HARMONY_DEMO_ANIMATION_H

#include <string>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./attribute_base.h"
#include "./css_property.h"
#include "helper/Optional.h"
#include "runtime/cssom/stylesheet/params/transform_param//transform_param.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

using KeyframeValue = std::variant<Dimension, double, std::shared_ptr<TransformParam>>;
// 为 KeyframeValue 定义相等性比较操作
inline bool operator==(const KeyframeValue& lhs, const KeyframeValue& rhs) {
    if (lhs.index() != rhs.index()) {
        return false;
    }

    if (std::holds_alternative<Dimension>(lhs)) {
        return std::get<Dimension>(lhs) == std::get<Dimension>(rhs);
    } else if (std::holds_alternative<double>(lhs)) {
        return std::get<double>(lhs) == std::get<double>(rhs);
    } else if (std::holds_alternative<std::shared_ptr<TransformParam>>(lhs)) {
        return *std::get<std::shared_ptr<TransformParam>>(lhs) == *std::get<std::shared_ptr<TransformParam>>(rhs);
    } else if (std::holds_alternative<Dimension>(lhs)) {
        return std::get<Dimension>(lhs) == std::get<Dimension>(rhs);
    }
    return false;
}
inline bool operator!=(const KeyframeValue& lhs, const KeyframeValue& rhs) {
    return !(lhs == rhs);
}

struct KeyFrame {
    float percent = 0;
    std::vector<std::pair<CSSProperty::Type, KeyframeValue>> params;
};
// 为 KeyFrame 定义相等性比较操作
inline bool operator==(const KeyFrame& lhs, const KeyFrame& rhs) {
    return lhs.percent == rhs.percent && lhs.params == rhs.params;
}
inline bool operator!=(const KeyFrame& lhs, const KeyFrame& rhs) {
    return !(lhs == rhs);
}

struct AnimationOption {
    std::string name;
    std::vector<KeyFrame> keyframes;
    int32_t duration = 0;
    std::string timingFunction = "ease";
    uint32_t delay = 0;
    int32_t interationCount = 1;
    std::string fill_mode = "none";
    std::string direction = "normal";
};

inline bool operator==(const AnimationOption& lhs, const AnimationOption& rhs) {
    return lhs.duration == rhs.duration &&
           lhs.timingFunction == rhs.timingFunction &&
           lhs.delay == rhs.delay &&
           lhs.interationCount == rhs.interationCount &&
           lhs.fill_mode == rhs.fill_mode &&
           lhs.direction == rhs.direction &&
           lhs.keyframes.size() == rhs.keyframes.size() &&
           lhs.name == rhs.name;
}
inline bool operator!=(const AnimationOption& lhs, const AnimationOption& rhs) {
    return !(lhs == rhs);
}

class Animation : public AttributeBase<AnimationOption> {
    public:
    Animation() = default;
    Animation(const napi_value&);
    void setValueFromNapi(const napi_value& value) override {};
    void setValueFromStringView(std::string_view value) override {};
    void setNameFromNapi(const napi_value&);
    void setKeyFramesFromNapi(const napi_value&);
    void setDurationFromNapi(const napi_value&);
    void setTimingFunctionFromNapi(const napi_value&);
    void setFillModeFromNapi(const napi_value&);
    void setDelayFromNapi(const napi_value&);
    void setIterationCountFromNapi(const napi_value&);
    void setIntoNode(ArkUI_NodeHandle& node);
    static void staticSetValueToNode(const ArkUI_NodeHandle& node,
                                     const AnimationOption& value);
    static void staticResetValueToNode(const ArkUI_NodeHandle& node);

    private:
    AnimationOption item_;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_ANIMATION_H
