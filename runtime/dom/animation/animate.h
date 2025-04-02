/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <list>
#include <memory>
#include <string>

#include "runtime/dom/animation/animator/animator.h"
#include "runtime/dom/animation/props_controller.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroAnimate : public TaroHelper::TaroClassLife<TaroAnimate> {
        public:
        explicit TaroAnimate(std::weak_ptr<TaroDOM::TaroRenderNode> node, TaroAnimateType type,
                             std::shared_ptr<TaroAnimationPropsController> props_controller)
            : animate_type_(type),
              node_owner_(node),
              props_controller_(props_controller) {}
        virtual ~TaroAnimate() {}

        // vsync进行tick计算
        virtual void tick(uint64_t current_time);

        // 判断当前animate是否需要继续tick
        virtual bool isActive();

        // 暂停动画
        virtual void pause();

        // 恢复动画
        virtual void resume();

        // 是否是有效的动画id
        virtual bool isValid(uint32_t animator_id);

        // 异步删除动画
        void asynDeleteAnimator(uint32_t animator_id);

        // 同步删除动画
        void deleteAnimator(uint32_t animator_id);

        // 清空动画
        void clearAnimator();

        std::shared_ptr<TaroAnimator> getAnimatorById(uint32_t animator_id);

        protected:
        // 添加动画
        void addAnimator(std::shared_ptr<TaroAnimator> animator);

        // 重置系统值
        void resetStyleProp(CSSProperty::Type prop_type);

        TaroAnimateType animate_type_ = TaroAnimateType::INVALID;
        std::weak_ptr<TaroDOM::TaroRenderNode> node_owner_;
        std::shared_ptr<TaroAnimationPropsController> props_controller_ = nullptr;

        private:
        // 指向动画删除操作
        void executeDelete();

        // 属性重置为系统值
        void resetStyleSheetProp();

        // std::mutex animators_lock_;
        std::list<std::shared_ptr<TaroAnimator>> animators_;
        // std::mutex change_lock_;
        std::list<uint32_t> delete_queue_;

        // 需要被重新设置为系统值的属性
        std::unordered_set<CSSProperty::Type> set_system_props_;
    };

} // namespace TaroAnimate
} // namespace TaroRuntime
