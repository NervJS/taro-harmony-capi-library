/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "css_transition.h"

#include "./animator/animation.h"
#include "./animator/curve_animation.h"
#include "./animator/keyframe_animation.h"
#include "animation_config.h"
#include "runtime/dom/animation/animation_lock.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroCSSTransition::TaroCSSTransition(
        std::shared_ptr<TaroAnimationPropsController> props_controller,
        std::weak_ptr<TaroDOM::TaroRenderNode> node)
        : TaroAnimate(node, TaroAnimateType::CSS_TRANSITION, props_controller) {}

    int TaroCSSTransition::bindTransition(const const TaroCSSOM::TaroStylesheet::TransitionParam& param) {
        std::shared_ptr<TaroAnimator> animator = std::make_shared<TaroAnimator>();
        animator->setDuration(param.duration_);
        animator->setDelay(param.delay);
        animator->setFillMode(TaroAnimationFillMode::Forwards);

        animator->setStopCallBack(
            std::bind(&TaroCSSTransition::stopCallBack, this, animator->id()));

        auto prop_type = param.prop_type_;
        auto set_fun =
            std::bind(&TaroAnimationPropsController::setProp, props_controller_,
                      animate_type_, prop_type, animator->id(), std::placeholders::_1);

        auto node = node_owner_.lock();

        TaroAnimationPropValue start_value;
        bool success = TaroAnimationProps::getSystemPropValue(
            prop_type, node_owner_.lock(), start_value);
        if (!success) {
            TARO_LOG_ERROR("TaroTransition",
                           "prop:%{public}d system value is not found", prop_type);
            return -1;
        }

        TaroAnimationPropValue end_value;
        success = TaroAnimationProps::getAnimationPropValue(node,
                                                            prop_type, param.prop_value_, start_value, end_value);
        if (!success) {
            TARO_LOG_ERROR("TaroAnimation",
                           "get prop:%{public}d value failed", prop_type);
            return -2;
        }

        auto animation = createCurveAnimation(
            prop_type, start_value, end_value,
            TaroCurves::getCurve(param.timing_function_), set_fun);

        animator->addAnimation(animation);
        addAnimator(animator);
        type_to_animator_.emplace_back(std::make_pair(prop_type, animator));
        animator->play();
        return 0;
    }

    void TaroCSSTransition::stopCallBack(int32_t anim_id) {
        TARO_LOG_DEBUG("TaroAnimation", "stop anim_id:%{public}d", anim_id);
        std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
        std::weak_ptr<TaroCSSTransition> weak_this = shared_from_this();
        runner->runTask(TaroThread::TaskThread::MAIN, [weak_this, anim_id] {
            auto tmp_this = weak_this.lock();
            if (tmp_this) {
                std::lock_guard lock(AnimationLock::getAnimationLock());
                tmp_this->asynDeleteAnimator(anim_id);
            }
        });
    }

    void TaroCSSTransition::clearAnimId(CSSProperty::Type prop_type,
                                        int32_t anim_id) {
        props_controller_->clearAnimId(prop_type, anim_id);
    }

    void TaroCSSTransition::clear(CSSProperty::Type prop_type) {
        for (auto iter = type_to_animator_.begin();
             iter != type_to_animator_.end();) {
            if (iter->first == prop_type) {
                asynDeleteAnimator(iter->second->id());
                iter = type_to_animator_.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void TaroCSSTransition::getRunningProp(std::vector<CSSProperty::Type>& props) {
        for (const auto& elem : type_to_animator_) {
            props.push_back(elem.first);
        }
    }

    void TaroCSSTransition::cancelTransition(CSSProperty::Type prop_type) {
        for (const auto& elem : type_to_animator_) {
            if (elem.first == prop_type) {
                elem.second->stop();
                resetStyleProp(prop_type);
            }
        }
    }

    void TaroCSSTransition::refreshAnimation() {
        for (const auto& elem : type_to_animator_) {
            TaroAnimationProps::resetStyleSheetProp(node_owner_, elem.first);
        }
        clearAnimator();
        type_to_animator_.clear();
    }

    std::shared_ptr<TaroAnimation> TaroCSSTransition::createCurveAnimation(
        CSSProperty::Type prop_type, const TaroAnimationPropValue& begin_value,
        const TaroAnimationPropValue& end_value, std::shared_ptr<TaroCurve> curve,
        const TaroAnimationPropSetFun& set_fun) {
        // check type
        if (begin_value.index() != end_value.index()) {
            TARO_LOG_ERROR("TaroAnimation", "begin and value types are not same");
            return nullptr;
        }

        bool b_ok = TaroAnimationProps::checkPropertyType(prop_type, begin_value);
        if (!b_ok) {
            TARO_LOG_ERROR("TaroAnimation", "invalid prop type");
            return nullptr;
        }
        auto evaluator = TaroAnimationProps::getEvaluator(prop_type);

#define CREATE_CURVE_ANIMATION(PROP_TYPE)                                    \
    {                                                                        \
        const PROP_TYPE* begin_float = std::get_if<PROP_TYPE>(&begin_value); \
        const PROP_TYPE* end_float = std::get_if<PROP_TYPE>(&end_value);     \
        if (begin_float != nullptr && end_float != nullptr) {                \
            std::shared_ptr<TaroCurveAnimation<PROP_TYPE>> animation =       \
                std::make_shared<TaroCurveAnimation<PROP_TYPE>>(             \
                    *begin_float, *end_float, *end_float, curve, set_fun);   \
            animation->setEvaluator(evaluator);                              \
            return animation;                                                \
        }                                                                    \
    }
        CREATE_CURVE_ANIMATION(double);
        CREATE_CURVE_ANIMATION(
            std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>);
        CREATE_CURVE_ANIMATION(Dimension);
        return nullptr;
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
