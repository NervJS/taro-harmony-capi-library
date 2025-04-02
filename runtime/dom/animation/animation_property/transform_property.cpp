/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "transform_property.h"

#include "../animation_property.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/cssom/stylesheet/params/transform_param/transform_param.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropTransform::AnimationPropTransform(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type,
                            typeid(TaroCSSOM::TaroStylesheet::Transform).name()) {}

    bool AnimationPropTransform::getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                    CSSProperty::Type prop_type,
                                                    TaroAnimationPropValue &prop_value) const {
        auto transform = node->GetTransform();
        if (!transform.has_value()) {
            prop_value = TaroCSSOM::TaroStylesheet::TransformParam::staticTransformSystemValue();
        } else {
            float width = node->layoutDiffer_.computed_style_.width;
            float height = node->layoutDiffer_.computed_style_.height;
            prop_value = std::make_shared<TaroCSSOM::TaroStylesheet::TransformParam>(
                TaroCSSOM::TaroStylesheet::TransformParam::staticTransformItemsPercent(transform.value(), width, height));
        }
        return true;
    }

    bool AnimationPropTransform::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
        const TaroAnimationPropValue &sys_value,
        TaroAnimationPropValue &prop_value) const {
        if (auto transformVal = std::get_if<
                std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>>(
                &n_val)) {
            if (*transformVal == nullptr) {
                TARO_LOG_ERROR("TaroAnimation", "invalid transform");
                return false;
            }
            float width = node->layoutDiffer_.computed_style_.width;
            float height = node->layoutDiffer_.computed_style_.height;
            prop_value = std::make_shared<TaroCSSOM::TaroStylesheet::TransformParam>(
                TaroCSSOM::TaroStylesheet::TransformParam::staticTransformItemsPercent(*(*transformVal), width, height));
            return true;
        }
        return false;
    }

    void AnimationPropTransform::setNodeProperty(
        std::shared_ptr<TaroDOM::TaroRenderNode> node, CSSProperty::Type prop_type,
        const TaroAnimationPropValue &prop_value) const {
        const auto *transform_value =
            std::get_if<std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>>(
                &prop_value);
        if (transform_value == nullptr) {
            return;
        }
        node->SetTransform(*(*transform_value));
    }

    void AnimationPropTransform::setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                   CSSProperty::Type prop_type,
                                                   const TaroCSSOM::TaroStylesheet::KeyframeValue &keyframe) const {
        const auto transform = std::get_if<std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>>(&keyframe);
        if (transform == nullptr) {
            return;
        }
        node->SetTransform(*(*transform));
    }

    void AnimationPropTransform::resetStyleSheetProp(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                     CSSProperty::Type prop_type,
                                                     TaroDOM::StylesheetRef style) const {
        node->ResetTransform();
    }

    ANIMATION_PROP_REGISTER(CSSProperty::Transform, AnimationPropTransform);
} // namespace TaroAnimate
} // namespace TaroRuntime
