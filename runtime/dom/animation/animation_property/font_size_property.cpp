/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "font_size_property.h"

#include "../animation_property.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroAnimate {
    AnimationPropFontSize::AnimationPropFontSize(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(static_cast<double>(0)).name()) {}

    bool AnimationPropFontSize::getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                   CSSProperty::Type prop_type,
                                                   TaroAnimationPropValue &prop_value) const {
        auto style = node->style_ref_;
        prop_value = 1.0f;
        if (!style->fontSize.has_value()) {
            return false;
        }
        auto fontSize = style->fontSize.value();
        auto fontSizeValue = fontSize.ParseToVp(node->GetDimensionContext());
        if (fontSizeValue.has_value()) {
            prop_value = fontSizeValue.value();
            return true;
        } else {
            return false;
        }
    }

    void AnimationPropFontSize::setNodeProperty(
        std::shared_ptr<TaroDOM::TaroRenderNode> node, CSSProperty::Type prop_type,
        const TaroAnimationPropValue &prop_value) const {
        const double *double_value = std::get_if<double>(&prop_value);
        if (double_value == nullptr) {
            return;
        }
        // TODO @朱总 fontsize
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setFontSize(
            node->GetArkUINodeHandle(), *double_value);
    }
    ANIMATION_PROP_REGISTER(CSSProperty::Type::FontSize, AnimationPropFontSize);
} // namespace TaroAnimate
} // namespace TaroRuntime
