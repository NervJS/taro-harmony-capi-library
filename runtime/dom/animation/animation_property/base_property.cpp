/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "base_property.h"

#include "runtime/cssom/stylesheet/common.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroAnimate {
    AnimationPropBase::AnimationPropBase(CSSProperty::Type prop_type,
                                         const std::string &anim_type_name)
        : prop_type_(prop_type), anim_type_name_(anim_type_name) {
        evaluator_ = TaroEvaluators::getEvaluator(TaroEvaluatorType::Base);
    }

    bool AnimationPropBase::checkPropertyType(
        CSSProperty::Type prop_type,
        const TaroAnimationPropValue &prop_value) const {
        // check double
        if (anim_type_name_ == typeid(static_cast<double>(0)).name()) {
            return std::holds_alternative<double>(prop_value);
        }

        // check TaroCSSOM::TaroStylesheet::Transform
        if (anim_type_name_ == typeid(TaroCSSOM::TaroStylesheet::Transform).name()) {
            return std::holds_alternative<std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>>(prop_value);
        }

        // check tlength
        if (anim_type_name_ == typeid(Dimension).name()) {
            return std::holds_alternative<Dimension>(prop_value);
        }
        return false;
    }

    bool AnimationPropBase::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
        const TaroAnimationPropValue &sys_value,
        TaroAnimationPropValue &prop_value) const {
        if (auto val = std::get_if<Dimension>(&n_val)) {
            if (auto vpVal = val->ParseToVp(node->GetDimensionContext()); vpVal.has_value()) {
                prop_value = vpVal.value();
                return true;
            } else if (val->Unit() == DimensionUnit::PERCENT) {
                return getFloatPercentValue(val->Value(), sys_value, prop_value);
            }
        }
        return false;
    }

    bool AnimationPropBase::getFloatPercentValue(
        const double float_percent, const TaroAnimationPropValue &sys_value,
        TaroAnimationPropValue &prop_value) const {
        if (float_percent < 0.0f) {
            return false;
        }
        const double *double_sys = std::get_if<double>(&sys_value);
        if (double_sys == nullptr) {
            return false;
        }
        prop_value = evaluator_->evaluate(0.0f, *double_sys, float_percent / 100);
        return true;
    }

    bool AnimationPropBase::getFloatPropValue(
        CSSProperty::Type prop_type,
        const Dimension &val,
        const double sys_value, double &prop_value) const {
        if (auto vpVal = val.ParseToVp(); vpVal.has_value()) {
            prop_value = vpVal.value();
            return true;
        } else if (val.Unit() == DimensionUnit::PERCENT) {
            return getFloatPercentValue(val.Value(), sys_value, prop_value);
        }

        return false;
    }

    bool AnimationPropBase::getFloatPercentValue(const double float_percent,
                                                 const double sys_value,
                                                 double &prop_value) const {
        if (float_percent < 0.0f) {
            return false;
        }

        prop_value = evaluator_->evaluate(0.0f, sys_value, float_percent / 100);
        return true;
    }

    std::shared_ptr<TaroEvaluator> AnimationPropBase::getEvaluator() const {
        return evaluator_;
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
