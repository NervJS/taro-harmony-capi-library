#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_set>

#include "animation.h"
#include "helper/life_statistic.h"
#include "runtime/dom/animation/curve/curve.h"

namespace TaroRuntime {
namespace TaroAnimate {

    enum class TaroAnimatorType {
        CSS_ANIMATION = 1,
        CSS_TRANSITION = 2,
        JS_ANIMATION = 3
    };

    class TaroAnimator : public TaroHelper::TaroClassLife<TaroAnimator> {
        public:
        using StopCallBackFun = std::function<void()>;
        using IterCallBackFun = std::function<void(int32_t iteration)>;

        public:
        TaroAnimator();
        virtual ~TaroAnimator();
        // set config
        // void setCurve(std::shared_ptr<TaroCurve> curve);
        void setDirection(TaroAnimationDirection direction);
        void setFillMode(TaroAnimationFillMode fill_mode);
        TaroAnimationFillMode getFillMode() {
            return fill_mode_;
        }
        void setIteration(int32_t iteration);
        void setDelay(int32_t delay);
        void setDuration(int32_t duration);
        void setName(const std::string& name);
        void setStopCallBack(const StopCallBackFun& stop_fun);
        void setIterCallBack(const IterCallBackFun& iter_fun);
        uint32_t id();
        TaroAnimationState state() const {
            return state_;
        }

        void play();
        void stop();
        void pause();
        void resume();
        void cancel();
        void onFrame(int64_t frame_time);
        void addAnimation(std::shared_ptr<TaroAnimation> animation);
        void addPropType(CSSProperty::Type prop_type);
        bool isActivePropType(CSSProperty::Type prop_type);
        const std::unordered_set<CSSProperty::Type>& getProps() {
            return active_props_;
        }

        // 媒体查询时删除动画桢信息，更换为新的桢
        void clearAnimation();

        // 重新计算状态等各类信息
        void refresh();

        void startInner(bool always_notify);
        bool getInitAnimationDirection();

        protected:
        float getNormalizedTime(float played_time, bool needStop);
        void updateRepeatTimesLeft(int64_t elapse_time);

        std::list<std::shared_ptr<TaroAnimation>> animations_;
        int32_t iteration_ = 1; // 重复次数
        TaroAnimationDirection direction_ = TaroAnimationDirection::Normal;
        uint32_t start_delay_ = 0; // 设置动画在启动前的延迟间隔,ms
        int32_t duration_ = 0;     // 持续时间
        TaroAnimationFillMode fill_mode_ = TaroAnimationFillMode::None;
        std::list<StopCallBackFun> stop_callback_funs_;
        IterCallBackFun iter_callback_fun_ = nullptr;
        std::string name_;

        // 运行过程汇总状态
        bool is_reverse_ = false;
        bool is_resume_ = false;
        bool is_cur_direction_ = false;
        int32_t iteration_left_ = 0;
        TaroAnimationState state_ = TaroAnimationState::IDLE;
        int64_t start_time_ = 0; // 开始时间
        int64_t pause_start_time = 0;
        int64_t pause_total_time_ = 0; // 总共暂停时间
        int64_t elapse_time_ = 0;      // 已经流逝的总时间

        uint32_t anim_id_ = 0;
        std::unordered_set<CSSProperty::Type> active_props_;
    };

    using TaroAnimatorPtr = std::shared_ptr<TaroAnimator>;
} // namespace TaroAnimate
} // namespace TaroRuntime
