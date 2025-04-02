/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "draw_number_property.h"

#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropDrawNumber::AnimationPropDrawNumber(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(static_cast<double>(0)).name()) {
    }

    bool AnimationPropDrawNumber::getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                     CSSProperty::Type prop_type,
                                                     TaroAnimationPropValue& prop_value) const {
        switch (prop_type) {
            case CSSProperty::Type::Opacity: {
                const auto& opacity_value = node->GetOpacity();
                prop_value = static_cast<double>(opacity_value.value_or(1));
                return true;
            } break;
            default:
                return false;
        }
        return false;
    }

    bool AnimationPropDrawNumber::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue& n_val,
        const TaroAnimationPropValue& sys_value,
        TaroAnimationPropValue& prop_value) const {
        if (auto val = std::get_if<double>(&n_val)) {
            prop_value = *val;
            return true;
        }
        return false;
    }

    void AnimationPropDrawNumber::setNodeProperty(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                  CSSProperty::Type prop_type,
                                                  const TaroAnimationPropValue& prop_value) const {
        const double* double_value = std::get_if<double>(&prop_value);
        if (double_value == nullptr) {
            return;
        }
        TARO_LOG_DEBUG("TaroAnimation", "prop:%{public}d value:%{public}f", prop_type, *double_value);
        switch (prop_type) {
            case CSSProperty::Type::Opacity:
                node->SetOpacity(static_cast<float>(*double_value));
                break;
            default:
                break;
        }
    }

    // 设置动画初始值到节点
    void AnimationPropDrawNumber::setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                    CSSProperty::Type prop_type,
                                                    const TaroCSSOM::TaroStylesheet::KeyframeValue& keyframe) const {
        auto int_value = std::get_if<double>(&keyframe);
        if (int_value == nullptr) {
            return;
        }
        switch (prop_type) {
            case CSSProperty::Type::Opacity:
                node->SetOpacity(static_cast<float>(*int_value));
                break;
            default:
                break;
        }
    }

    // 设置style值到node节点
    void AnimationPropDrawNumber::resetStyleSheetProp(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                      CSSProperty::Type prop_type,
                                                      TaroDOM::StylesheetRef style) const {
        switch (prop_type) {
            case CSSProperty::Type::Opacity:
                node->SetOpacity(style->opacity.has_value() ? style->opacity.value() : 1);
                break;
            default:
                break;
        }
    }

    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Opacity, AnimationPropDrawNumber, 1);
} // namespace TaroAnimate
} // namespace TaroRuntime
