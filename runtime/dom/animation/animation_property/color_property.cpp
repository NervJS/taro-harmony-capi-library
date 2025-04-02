/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "color_property.h"

#include "../animation_property.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropColor::AnimationPropColor(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(static_cast<double>(0)).name()) {
        evaluator_ = TaroEvaluators::getEvaluator(TaroEvaluatorType::Color);
    }

    bool AnimationPropColor::getSystemPropValue(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            TaroAnimationPropValue &prop_value) const {
        auto style = node->style_ref_;
        prop_value = static_cast<double>(style->color.value_or(0));
        return style->color.has_value();
    }

    void AnimationPropColor::setNodeProperty(
        std::shared_ptr<TaroDOM::TaroRenderNode> node, CSSProperty::Type prop_type,
        const TaroAnimationPropValue &prop_value) const {
        const double *double_value = std::get_if<double>(&prop_value);
        if (double_value == nullptr) {
            return;
        }
        // TODO @朱总 color
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setColor(
            node->GetArkUINodeHandle(), *double_value);
    }
    ANIMATION_PROP_REGISTER(CSSProperty::Color, AnimationPropColor);
} // namespace TaroAnimate
} // namespace TaroRuntime
