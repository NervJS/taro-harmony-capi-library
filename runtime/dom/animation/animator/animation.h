/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "evaluators.h"
#include "helper/life_statistic.h"

namespace TaroRuntime {
namespace TaroAnimate {
    enum class TaroAnimationDirection {
        Normal = 0,  // 正常播放
        Reverse = 1, // 反向播放
        Alternate =
            2, // 动画在每次迭代时交替进行，即奇数次迭代正常播放，偶数次迭代反向播放
        Alternate_Reverse =
            3 // 动画在每次迭代时交替进行，但开始是反向播放，然后是正常播放
    };

    enum class TaroAnimationState {
        IDLE,    // when animation not start or been cancel.
        RUNNING, // play in reverse / forward direction.
        PAUSED,  // paused by call Pause API.
        STOPPED, // stopped by call Finish/Stop API or has played to the end.
    };

    enum class TaroAnimationFillMode {
        None =
            0, // 动画在播放前和播放后都不会对元素应用任何样式。元素将保持其原始样式
        Forwards =
            1,         // 动画结束后，元素将保持动画的最后一帧的样式。
                       // 这意味着动画结束后，元素将不再是其原始样式，而是动画结束时的样式。
        Backwards = 2, // 动画在播放前（即在等待期），元素将呈现动画的第一帧的样式。
                       // 这意味着元素在开始动画之前就已经有了动画的一部分样式
        Both = 3,      // 同时具有 forwards 和 backwards 的效果
    };

    class TaroAnimation : public TaroHelper::TaroClassLife<TaroAnimation> {
        public:
        TaroAnimation() {
            evaluator_ = TaroEvaluators::getEvaluator(TaroEvaluatorType::Base);
        };
        virtual ~TaroAnimation() = default;
        virtual void onNormalizedTimestampChanged(float normalized, bool reverse) = 0;
        virtual void setSysValue() {}
        virtual void setStartValue(bool is_reverse) {}
        void setEvaluator(std::shared_ptr<TaroEvaluator> evaluator) {
            evaluator_ = evaluator;
        }

        protected:
        std::shared_ptr<TaroEvaluator> evaluator_ = nullptr;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
