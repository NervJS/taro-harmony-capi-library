#include "animator.h"

#include <sys/stat.h>

#include "animation.h"
#include "utils.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroAnimator::TaroAnimator() {
        static uint32_t g_amid_id = 0;
        ++g_amid_id;
        anim_id_ = g_amid_id;
    }

    TaroAnimator::~TaroAnimator() {}

    void TaroAnimator::setDirection(TaroAnimationDirection direction) {
        direction_ = direction;
    }

    void TaroAnimator::setFillMode(TaroAnimationFillMode fill_mode) {
        fill_mode_ = fill_mode;
    }

    void TaroAnimator::setIteration(int32_t iteration) {
        if (iteration < 0) {
            iteration = 10000000;
        }
        iteration_ = iteration;
    }

    void TaroAnimator::setDelay(int32_t delay) {
        start_delay_ = delay;
    }

    void TaroAnimator::setDuration(int32_t duration) {
        duration_ = duration;
    }
    void TaroAnimator::setName(const std::string &name) {
        name_ = name;
    }

    void TaroAnimator::setStopCallBack(const StopCallBackFun &stop_fun) {
        stop_callback_funs_.emplace_back(stop_fun);
    }
    void TaroAnimator::setIterCallBack(const IterCallBackFun &iter_fun) {
        iter_callback_fun_ = iter_fun;
    }

    uint32_t TaroAnimator::id() {
        return anim_id_;
    }

    void TaroAnimator::play() {
        if (iteration_ == 0) {
            return;
        }

        startInner(false);
        start_time_ = TaroUtils::getCurrentMsTime();
    }

    void TaroAnimator::startInner(bool always_notify) {
        iteration_left_ = iteration_;
        is_cur_direction_ = getInitAnimationDirection();
        state_ = TaroAnimationState::RUNNING;
        // todo: 触发启动事件
    }

    float TaroAnimator::getNormalizedTime(float played_time, bool need_stop) {
        float normalized_time = 0.0f;
        if (need_stop) {
            switch (fill_mode_) {
                case TaroAnimationFillMode::Forwards:
                    // Fall through.
                case TaroAnimationFillMode::Both:
                    normalized_time = 1.0f;
                    break;
                case TaroAnimationFillMode::None:
                    // Fall through.
                case TaroAnimationFillMode::Backwards:
                    normalized_time = 0.0f;
                    break;
                default:
                    normalized_time = 1.0f;
                    break;
            }
        } else {
            normalized_time = played_time / duration_;
        }
        return is_cur_direction_ ? 1.0f - normalized_time : normalized_time;
    }

    void TaroAnimator::updateRepeatTimesLeft(int64_t elapse_time) {
        if (iteration_left_ == 0) {
            return;
        }
        auto iteration = elapse_time / duration_;
        while (iteration_left_ + iteration > iteration_) {
            --iteration_left_;
            if (iteration_left_ <= 0) {
                return;
            }

            // 触发iterator事件
            if (iter_callback_fun_ != nullptr) {
                iter_callback_fun_(iteration_ - iteration_left_);
            }
            is_cur_direction_ = getInitAnimationDirection();
        }
    }

    void TaroAnimator::onFrame(int64_t frame_time) {
        if (state_ != TaroAnimationState::RUNNING || frame_time < start_time_) {
            return;
        }

        // 处理暂停阶段需要设置值的情况
        if (start_time_ + start_delay_ + pause_total_time_ > frame_time) {
            if (fill_mode_ == TaroAnimationFillMode::Backwards ||
                fill_mode_ == TaroAnimationFillMode::Both) {
                for (auto &animation : animations_) {
                    animation->setStartValue(is_reverse_);
                }
            }
            return;
        }

        int64_t elapse_time =
            frame_time - start_time_ - start_delay_ - pause_total_time_;
        // 更新repeat & 触发repeat事件
        updateRepeatTimesLeft(elapse_time);

        elapse_time_ = elapse_time;
        float normalized_time =
            getNormalizedTime(elapse_time % duration_, iteration_left_ == 0);

        for (const auto &animation : animations_) {
            animation->onNormalizedTimestampChanged(normalized_time, is_reverse_);
        }

        // 更新stop & 触发stop事件
        if (iteration_left_ == 0) {
            stop();
        }
    }

    void TaroAnimator::pause() {
        if (state_ == TaroAnimationState::RUNNING) {
            state_ = TaroAnimationState::PAUSED;
            pause_start_time = TaroUtils::getCurrentMsTime();
        }
    }

    void TaroAnimator::resume() {
        if (state_ == TaroAnimationState::PAUSED) {
            state_ = TaroAnimationState::RUNNING;
            pause_total_time_ += (TaroUtils::getCurrentMsTime() - pause_start_time);
        }
    }

    void TaroAnimator::stop() {
        if (iteration_ == 0 || state_ == TaroAnimationState::STOPPED) {
            return;
        }

        // 设置fill mode
        if (fill_mode_ == TaroAnimationFillMode::None ||
            fill_mode_ == TaroAnimationFillMode::Backwards) {
            for (auto &animation : animations_) {
                // animation->setSysValue();
            }
        }

        // 触发stop事件
        state_ = TaroAnimationState::STOPPED;
        for (auto &stop_callback_fun : stop_callback_funs_) {
            stop_callback_fun();
        }
    }

    bool TaroAnimator::getInitAnimationDirection() {
        if (direction_ == TaroAnimationDirection::Normal) {
            return is_reverse_;
        }
        if (direction_ == TaroAnimationDirection::Reverse) {
            return !is_reverse_;
        }
        // for Alternate and Alternate_Reverse
        bool isOddRound = ((iteration_ - iteration_left_ + 1) % 2) == 1;
        bool odd_normal = direction_ == TaroAnimationDirection::Alternate;
        if (isOddRound != odd_normal) {
            return !is_reverse_;
        }
        return is_reverse_;
    }

    void TaroAnimator::addAnimation(std::shared_ptr<TaroAnimation> animation) {
        animations_.emplace_back(animation);
    }

    void TaroAnimator::addPropType(CSSProperty::Type prop_type) {
        active_props_.insert(prop_type);
    }

    bool TaroAnimator::isActivePropType(CSSProperty::Type prop_type) {
        if (state_ == TaroAnimationState::STOPPED && fill_mode_ != TaroAnimationFillMode::Forwards && fill_mode_ != TaroAnimationFillMode::Both) {
            return false;
        }
        return active_props_.count(prop_type) > 0;
    }

    void TaroAnimator::clearAnimation() {
        animations_.clear();
        active_props_.clear();
    }

    void TaroAnimator::refresh() {
        auto cur_time = TaroUtils::getCurrentMsTime();

        // 动画处于暂停状态
        uint64_t tmp_time = ((uint64_t)duration_) * iteration_;
        if (state_ == TaroAnimationState::PAUSED) {
            if (start_time_ + start_delay_ + tmp_time + pause_total_time_ < pause_start_time) { // 动画已停止
                state_ = TaroAnimationState::STOPPED;
                elapse_time_ = start_delay_ + tmp_time;
            } else { // 动画未停止
                elapse_time_ = pause_start_time - start_time_ - start_delay_ - pause_total_time_;
            }
        } else {
            if (start_time_ + start_delay_ + tmp_time + pause_total_time_ < cur_time) {
                state_ = TaroAnimationState::STOPPED;
                elapse_time_ = start_delay_ + tmp_time;
            } else { // 动画未停止
                elapse_time_ = cur_time - start_time_ - start_delay_ - pause_total_time_;
                state_ = TaroAnimationState::RUNNING;
            }
        }
        iteration_left_ = iteration_;

        updateRepeatTimesLeft(elapse_time_);
        if (state_ == TaroAnimationState::STOPPED &&
            (fill_mode_ == TaroAnimationFillMode::Forwards || fill_mode_ == TaroAnimationFillMode::Both)) {
            for (const auto &animation : animations_) {
                animation->onNormalizedTimestampChanged(1.0, is_reverse_);
            }
        } else if (state_ != TaroAnimationState::STOPPED) {
            float normalized_time = getNormalizedTime(elapse_time_ % duration_, iteration_left_ == 0);
            for (const auto &animation : animations_) {
                animation->onNormalizedTimestampChanged(1.0, is_reverse_);
            }
        }
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
