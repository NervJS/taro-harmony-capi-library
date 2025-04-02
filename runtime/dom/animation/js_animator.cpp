/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "js_animator.h"

#include "./animator/curve_animation.h"
#include "js_animation.h"
#include "runtime/dom/animation/animation_property.h"
#include "runtime/render.h"
#include "thread/TaskExecutor.h"

namespace TaroRuntime {
namespace TaroAnimate {

    TaroJsAnimator::TaroJsAnimator(std::weak_ptr<TaroDOM::TaroRenderNode> node,
                                   std::weak_ptr<TaroJSAnimation> owner,
                                   std::shared_ptr<TaroAnimationPropsController> props_controller)
        : node_owner_(node), owner_(owner), props_controller_(props_controller) {
        static uint32_t g_id = 0;
        g_id++;
        id_ = g_id;
    }

    int TaroJsAnimator::setAnimation(std::shared_ptr<TaroJsAnimationOption> option) {
        option_ = option;
        step_index_ = -1;
        return switchNextStep();
    }

    int TaroJsAnimator::switchNextStep() {
        step_index_++;
        if (step_index_ >= option_->steps_.size()) {
            return 0;
        }

        cur_animator_ = nullptr;
        const auto &param = option_->steps_[step_index_];
        cur_animator_ = std::make_shared<TaroAnimator>();
        cur_animator_->setDelay(param.delay_);
        cur_animator_->setDuration(param.duration_);
        cur_animator_->setFillMode(TaroAnimationFillMode::Forwards);
        auto curve = TaroCurves::getCurve(param.timingFunction_);
        auto node = node_owner_.lock();

        for (auto const &keyframe : param.keyframes) {
            auto prop_type = keyframe.first;
            // 获取系统值
            TaroAnimationPropValue start_value;
            bool success = TaroAnimationProps::getSystemPropValue(
                prop_type, node, start_value);
            if (!success) {
                TARO_LOG_ERROR("TaroTransition",
                               "prop:%{public}d system value is not found", prop_type);
                return -1;
            }
            // 获取动画值
            TaroAnimationPropValue end_value;
            success = TaroAnimationProps::getAnimationPropValue(node,
                                                                prop_type, keyframe.second, start_value, end_value);
            if (!success) {
                TARO_LOG_ERROR("TaroAnimation",
                               "get prop:%{public}d value failed", prop_type);
                return -2;
            }

            // 创建curve动画
            auto set_fun =
                std::bind(&TaroAnimationPropsController::setProp, props_controller_,
                          TaroAnimateType::JS_ANIMATION, prop_type, id_, std::placeholders::_1);
            auto animation = createCurveAnimation(
                prop_type, start_value, end_value, curve, set_fun);
            if (animation == nullptr) {
                TARO_LOG_ERROR("TaroAnimation", "create animation failed");
                return -3;
            }
            cur_animator_->addAnimation(animation);
        }
        setTransformOrigin(param.transformOrigin_);
        cur_animator_->play();
        return 0;
    }

    int TaroJsAnimator::jumpNextStep() {
        step_index_++;
        if (step_index_ >= option_->steps_.size()) {
            // todo: wzq 动画终止，销毁资源
            return 0;
        }
        cur_animator_ = nullptr;

        // 将当前属性设置到节点
        const auto &param = option_->steps_[step_index_];
        for (auto const &keyframe : param.keyframes) {
            TaroAnimationProps::setKeyframeToNode(node_owner_.lock(), keyframe.first, keyframe.second);
        }
        return 0;
    }

    void TaroJsAnimator::tick(uint64_t current_time) {
        if (cur_animator_ == nullptr) {
            return;
        }
        cur_animator_->onFrame(current_time);
        if (cur_animator_->state() == TaroAnimationState::STOPPED) {
            auto runner = Render::GetInstance()->GetTaskRunner();
            auto owner = owner_.lock();
            int id = id_;
            if (step_index_ + 1 >= option_->steps_.size()) {
                runner->runTask(TaroThread::TaskThread::MAIN, [owner, id]() {
                    owner->stopCallBack(id);
                });
            } else {
                runner->runTask(TaroThread::TaskThread::MAIN, [owner, id]() {
                    owner->stepCallBack(id);
                });
            }
        }
    }

    void TaroJsAnimator::pause() {
        if (cur_animator_ != nullptr) {
            cur_animator_->pause();
        }
    }

    void TaroJsAnimator::resume() {
        if (cur_animator_ != nullptr) {
            cur_animator_->resume();
        }
    }

    void TaroJsAnimator::setTransformOrigin(const std::string &value) {
        TaroCSSOM::TaroStylesheet::TransformOrigin origin;
        origin.setValueFromStringView(value);
        auto node = node_owner_.lock();
        if (origin.has_value() && node != nullptr) {
            node->SetTransformOrigin(origin.value());
        }
    }

    std::shared_ptr<TaroAnimation> TaroJsAnimator::createCurveAnimation(
        CSSProperty::Type prop_type, const TaroAnimationPropValue &begin_value,
        const TaroAnimationPropValue &end_value, std::shared_ptr<TaroCurve> curve,
        const TaroAnimationPropSetFun &set_fun) {
        // check type
        if (begin_value.index() != end_value.index()) {
            TARO_LOG_ERROR("TaroAnimation", "begin and value types are not same");
            return nullptr;
        }

        bool b_ok = TaroAnimationProps::checkPropertyType(prop_type, begin_value);
        if (!b_ok) {
            TARO_LOG_ERROR("TaroAnimation", "invalid property type");
            return nullptr;
        }
        auto evaluator = TaroAnimationProps::getEvaluator(prop_type);

#define CREATE_CURVE_ANIMATION(PROP_TYPE)                                    \
    {                                                                        \
        const PROP_TYPE *begin_float = std::get_if<PROP_TYPE>(&begin_value); \
        const PROP_TYPE *end_float = std::get_if<PROP_TYPE>(&end_value);     \
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
