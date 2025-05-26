/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "animate.h"
#include "js_animator.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroJSAnimation final : public std::enable_shared_from_this<TaroJSAnimation>, public TaroAnimate {
        public:
        explicit TaroJSAnimation(
            std::shared_ptr<TaroAnimationPropsController> props_controller,
            std::weak_ptr<TaroDOM::TaroRenderNode> node)
            : TaroAnimate(node, TaroAnimateType::JS_ANIMATION, props_controller) {}
        ~TaroJSAnimation() {}

        int bindJsAnimation(std::shared_ptr<TaroJsAnimationOption> option);

        void tick(uint64_t current_time) override;
    
        // 判断当前animate是否需要继续tick
        virtual bool isActive();

        // 暂停动画
        void pause() override;

        // 恢复动画
        void resume() override;

        void stopCallBack(uint32_t id);

        void stepCallBack(uint32_t id);

        bool isValid(uint32_t animator_id) override;

        private:
        // 非当前动画animator下跳一个step
        void idleIterNext();

        std::list<TaroJsAnimatorPtr> js_animators_;
        TaroJsAnimatorPtr cur_animator_ = nullptr;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
