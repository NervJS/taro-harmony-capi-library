#pragma once

#include <list>

#include "animation.h"
#include "evaluator.h"
#include "key_frame.h"
#include "utils.h"

namespace TaroRuntime {
namespace TaroAnimate {
    template <typename T>
    class TaroKeyframeAnimation final : public TaroAnimation {
        public:
        using ValueSetFun = std::function<void(const T &t)>;

        public:
        explicit TaroKeyframeAnimation(const T &sys_value,
                                       const ValueSetFun &value_set_callback)
            : sys_value_(sys_value), value_set_callback_(value_set_callback) {}
        ~TaroKeyframeAnimation() = default;

        void addKeyframe(
            const std::list<std::shared_ptr<TaroKeyframe<T>>> &keyframes) {
            for (const auto &keyframe : keyframes) {
                AddKeyframe(keyframe);
            }
            // in order by time;
            keyframes_.sort([](const std::shared_ptr<TaroKeyframe<T>> &a,
                               const std::shared_ptr<TaroKeyframe<T>> &b) {
                return a->GetKeyTime() < b->GetKeyTime();
            });
        }

        void addKeyframe(const std::shared_ptr<TaroKeyframe<T>> &keyframe) {
            if (!keyframe) {
                TARO_LOG_ERROR("TaroAnimation", "add key frame failed. empty keyframe.");
                return;
            }
            if (!keyframe->isValid()) {
                TARO_LOG_ERROR("TaroAnimation",
                               "add key frame failed. invalid keyframe.");
                return;
            }
            auto iter = keyframes_.begin();
            for (; iter != keyframes_.end(); ++iter) {
                float val = (*iter)->getKeyTime();
                if (TaroUtils::nearEqual(val, keyframe->getKeyTime())) {
                    *iter = keyframe;
                    return;
                } else if (val > keyframe->getKeyTime()) {
                    break;
                }
            }
            keyframes_.insert(iter, keyframe);

            ++keyframe_num_;
        }

        void replaceKeyframe(
            const std::shared_ptr<TaroKeyframe<T>> &keyframe_replace) {
            if (!keyframe_replace) {
                TARO_LOG_ERROR("TaroAnimation", "add key frame failed. empty keyframe.");
                return;
            }
            if (!keyframe_replace->IsValid()) {
                TARO_LOG_ERROR("TaroAnimation",
                               "add key frame failed. invalid keyframe.");
                return;
            }
            for (auto &keyframe : keyframes_) {
                if (TaroUtils::nearEqual(keyframe->getKeyTime(),
                                         keyframe_replace->getKeyTime())) {
                    keyframe = keyframe_replace;
                }
            }
        }

        void setCurve(const std::shared_ptr<TaroCurve> &curve) {
            if (!curve) {
                TARO_LOG_ERROR("TaroAnimation", "set curve failed. curve is null.");
                return;
            }
            for (auto &keyframe : keyframes_) {
                keyframe->SetCurve(curve);
            }
        }

        void onNormalizedTimestampChanged(float normalized, bool reverse) {
            if (normalized < 0.0 || normalized > 1.0) {
                TARO_LOG_ERROR("TaroAnimatin",
                               "normalized time error. normalized: %{public}f",
                               normalized);
                return;
            }
            calculate(normalized);
            if (value_set_callback_ != nullptr) {
                value_set_callback_(current_value_);
            }
        }

        void setSysValue() {
            current_value_ = sys_value_;
            if (value_set_callback_ != nullptr) {
                value_set_callback_(current_value_);
            }
        }

        void setStartValue(bool reverse) {
            if (!keyframes_.empty()) {
                if (reverse) {
                    current_value_ = keyframes_.back()->getKeyValue();
                } else {
                    current_value_ = keyframes_.front()->getKeyValue();
                }
            }
            if (value_set_callback_ != nullptr) {
                value_set_callback_(current_value_);
            }
        }

        void calculate(float key_time) {
            if (keyframes_.empty()) {
                return;
            }
            auto begin = keyframes_.front()->getKeyValue();
            auto end = keyframes_.back()->getKeyValue();
            // The initial state is maintained when keyTime < 0.
            if (key_time < 0.0f) {
                current_value_ = begin;
                return;
            } else if (key_time > 1.0f || keyframe_num_ == 1) {
                // The final state is maintained when keyTime > 1 or keyframeNum_ = 1.
                current_value_ = end;
                return;
            }
            auto pre_keyframe = keyframes_.front();
            // Iteratively calculate the value between each keyframe.
            for (const auto &keyframe : keyframes_) {
                if (key_time < keyframe->getKeyTime()) {
                    float pre_key_time = pre_keyframe->getKeyTime();
                    if (TaroUtils::nearEqual(keyframe->getKeyTime(), pre_key_time)) {
                        return;
                    }
                    float intervalKeyTime =
                        (key_time - pre_key_time) / (keyframe->getKeyTime() - pre_key_time);
                    auto curve = keyframe->getCurve();
                    begin = pre_keyframe->getKeyValue();
                    end = keyframe->getKeyValue();
                    if (curve) {
                        current_value_ =
                            evaluator_->evaluate(begin, end, curve->move(intervalKeyTime));
                        return;
                    } else {
                        current_value_ = evaluator_->evaluate(
                            begin, end, TaroCurves::EASE->move(intervalKeyTime));
                        return;
                    }
                }
                pre_keyframe = keyframe;
            }
            current_value_ = end;
        }

        private:
        T current_value_;
        T sys_value_;
        int32_t keyframe_num_ = 0;
        ValueSetFun value_set_callback_ = nullptr;
        std::list<std::shared_ptr<TaroKeyframe<T>>> keyframes_;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
