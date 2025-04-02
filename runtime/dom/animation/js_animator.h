/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/dom/animation/animation_property.h"
#include "runtime/dom/animation/animator/animator.h"
#include "runtime/dom/animation/js_animation_param.h"
#include "runtime/dom/animation/props_controller.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroJSAnimation;
    class TaroJsAnimator final : public TaroHelper::TaroClassLife<TaroJsAnimator> {
        public:
        TaroJsAnimator(std::weak_ptr<TaroDOM::TaroRenderNode> node,
                       std::weak_ptr<TaroJSAnimation> owner,
                       std::shared_ptr<TaroAnimationPropsController> props_controller);
        ~TaroJsAnimator() = default;
        uint32_t id() const {
            return id_;
        }

        int setAnimation(std::shared_ptr<TaroJsAnimationOption> option);

        // 切到下一个animator动画
        int switchNextStep();

        // 切到下一帧
        int jumpNextStep();

        TaroAnimationState state() const {
            return state_;
        }

        // 暂停动画
        void pause();

        // 恢复动画
        void resume();

        void tick(uint64_t current_time);

        private:
        std::shared_ptr<TaroAnimation> createCurveAnimation(
            CSSProperty::Type prop_type, const TaroAnimationPropValue& begin_value,
            const TaroAnimationPropValue& end_value, std::shared_ptr<TaroCurve> curve,
            const TaroAnimationPropSetFun& set_fun);

        void setTransformOrigin(const std::string& value);

        private:
        // 存储各step
        std::shared_ptr<TaroAnimator> cur_animator_ = nullptr;
        int step_index_ = 0;
        std::shared_ptr<TaroJsAnimationOption> option_;

        std::weak_ptr<TaroDOM::TaroRenderNode> node_owner_;

        std::weak_ptr<TaroJSAnimation> owner_;

        std::shared_ptr<TaroAnimationPropsController> props_controller_ = nullptr;

        TaroAnimationState state_ = TaroAnimationState::IDLE;

        uint32_t id_ = 0;
    };

    using TaroJsAnimatorPtr = std::shared_ptr<TaroJsAnimator>;
} // namespace TaroAnimate
} // namespace TaroRuntime
