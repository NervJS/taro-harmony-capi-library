/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "draw_color_property.h"

#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropDrawColor::AnimationPropDrawColor(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(static_cast<double>(0)).name()) {
        evaluator_ = TaroEvaluators::getEvaluator(TaroEvaluatorType::Color);
    }

    bool AnimationPropDrawColor::getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                    CSSProperty::Type prop_type,
                                                    TaroAnimationPropValue& prop_value) const {
        Optional<uint32_t> sys_value;
        switch (prop_type) {
            case CSSProperty::Type::BackgroundColor:
                sys_value = node->GetBackgroundColor();
                break;
            case CSSProperty::Type::BorderTopColor:
                sys_value = node->GetBorderColor(TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::BorderLeftColor:
                sys_value = node->GetBorderColor(TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::BorderBottomColor:
                sys_value = node->GetBorderColor(TaroEdge::TaroEdgeBottom);
                break;
            case CSSProperty::Type::BorderRightColor:
                sys_value = node->GetBorderColor(TaroEdge::TaroEdgeRight);
                break;
            default:
                return false;
        }
        if (!sys_value.has_value()) {
            return false;
        }

        prop_value = static_cast<double>(sys_value.value());
        return true;
    }

    bool AnimationPropDrawColor::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue& n_val,
        const TaroAnimationPropValue& sys_value,
        TaroAnimationPropValue& prop_value) const {
        if (auto double_val = std::get_if<double>(&n_val)) {
            prop_value = *double_val;
            return true;
        }
        return false;
    }

    void AnimationPropDrawColor::setNodeProperty(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                 CSSProperty::Type prop_type,
                                                 const TaroAnimationPropValue& prop_value) const {
        const double* double_value = std::get_if<double>(&prop_value);
        if (double_value == nullptr) {
            return;
        }
        setColorValue(prop_type, node, static_cast<uint32_t>(*double_value));
    }

    void AnimationPropDrawColor::setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                   CSSProperty::Type prop_type,
                                                   const TaroCSSOM::TaroStylesheet::KeyframeValue& keyframe) const {
        if (!node) {
            return;
        }

        auto double_value = std::get_if<double>(&keyframe);
        if (double_value == nullptr) {
            return;
        }
        //         auto double_value = std::get_if<double>(length_value);
        //         if (double_value == nullptr) {
        //             return;
        //         }

        setColorValue(prop_type, node, static_cast<int32_t>(*double_value));
    }

    void AnimationPropDrawColor::setColorValue(CSSProperty::Type prop_type,
                                               std::shared_ptr<TaroDOM::TaroRenderNode>& node,
                                               uint32_t color_value) const {
        switch (prop_type) {
            case CSSProperty::Type::BackgroundColor:
                node->SetBackgroundColor(color_value);
                break;
            case CSSProperty::Type::BorderTopColor:
                node->SetBorderColor(color_value, TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::BorderLeftColor:
                node->SetBorderColor(color_value, TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::BorderBottomColor:
                node->SetBorderColor(color_value, TaroEdge::TaroEdgeBottom);
                break;
            case CSSProperty::Type::BorderRightColor:
                node->SetBorderColor(color_value, TaroEdge::TaroEdgeRight);
                break;
            default:
                break;
        }
    }

    // 设置style值到node节点
    void AnimationPropDrawColor::resetStyleSheetProp(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                     CSSProperty::Type prop_type,
                                                     TaroDOM::StylesheetRef style) const {
        switch (prop_type) {
            case CSSProperty::Type::BackgroundColor:
                node->ResetBackgroundColor();
                break;
            case CSSProperty::Type::BorderTopColor:
                node->ResetBorderColor(TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::BorderLeftColor:
                node->ResetBorderColor(TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::BorderRightColor:
                node->ResetBorderColor(TaroEdge::TaroEdgeRight);
                break;
            case CSSProperty::Type::BorderBottomColor:
                node->ResetBorderColor(TaroEdge::TaroEdgeBottom);
                break;
            default:
                break;
        }
    }

    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BackgroundColor, AnimationPropDrawColor, 1);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderTopColor, AnimationPropDrawColor, 2);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderLeftColor, AnimationPropDrawColor, 3);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderBottomColor, AnimationPropDrawColor, 4);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderRightColor, AnimationPropDrawColor, 5);
} // namespace TaroAnimate
} // namespace TaroRuntime
