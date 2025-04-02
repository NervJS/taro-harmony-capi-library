/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/css_animation.h"
#include "runtime/dom/animation/css_transition.h"
#include "runtime/dom/animation/js_animation.h"
#include "runtime/dom/node.h"
#include "trigger/frame_trigger.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroRenderNode;
}

namespace TaroAnimate {
    class TaroNodeAnimations final
        : public TaroFrameTrigger,
          public TaroHelper::TaroClassLife<TaroNodeAnimations> {
        public:
        explicit TaroNodeAnimations(std::shared_ptr<TaroDOM::TaroRenderNode> node);
        ~TaroNodeAnimations();

        // 根据style前后值清理已删除动画和过渡
        void clearStyleCSSAnimations();

        // 设置Animation初始化到node节点，方便系统进行后续yoga值计算
        void setCSSAnimationZeroValue();

        // 启动Animation动画
        int32_t startCSSAnimation();

        // 当属性变化时触发回调
        bool onSetPropertyIntoNode(const CSSProperty::Type &property,
                                   const TaroChange &changeType);

        // 绑定js动画
        int32_t bindJSAnimation(const napi_value &napi_val);

        int32_t bindJSAnimation(const TaroJsAnimationOptionRef &option);

        // 判断动画ID是否有效
        bool isValid(TaroAnimateType anim_type, int anim_id, uint64_t version);

        private:
        void tick(uint64_t current_time) override;

        void clearCSSAnimation();

        void clearCSSTransition();

        void pause() override;

        void resume() override;

        void enableTrigger();

        int bindAnimation(const TaroCSSOM::TaroStylesheet::AnimationOption &optional);

        int bindTransition(const TaroCSSOM::TaroStylesheet::TransitionParam &optional);

        void cancelTransition(CSSProperty::Type prop_type);

        void cancelAnimation(const std::string &name);

        // 重新刷新动画，因为折叠屏等原因
        void reloadAnimation(TaroDOM::StylesheetRef style);

        //         bool isValidVersion() const;

        // style是否设置了display: none
        bool needDisplay(const TaroCSSOM::TaroStylesheet::Display &display) const;

        private:
        std::shared_ptr<TaroCSSAnimation> css_animation_ = nullptr;

        std::shared_ptr<TaroCSSTransition> css_transition_ = nullptr;

        std::shared_ptr<TaroJSAnimation> js_animation_ = nullptr;

        std::shared_ptr<TaroAnimationPropsController> props_controller_ = nullptr;

        std::weak_ptr<TaroDOM::TaroRenderNode> node_owner_;

        uint64_t anim_version_ = 0;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
