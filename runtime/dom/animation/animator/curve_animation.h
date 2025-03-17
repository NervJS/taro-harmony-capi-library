#pragma once

#include <list>

#include "animation.h"
#include "evaluator.h"
#include "runtime/dom/animation/curve/curves.h"
#include "utils.h"

namespace TaroRuntime {
namespace TaroAnimate {
    template <typename T>
    class TaroCurveAnimation final : public TaroAnimation {
        public:
        using ValueSetFun = std::function<void(const T& t)>;

        public:
        TaroCurveAnimation(const T& begin, const T& end, const T& sys_value,
                           const std::shared_ptr<TaroCurve>& curve,
                           const ValueSetFun& value_set_callback)
            : sys_value_(sys_value),
              begin_(begin),
              end_(end),
              value_set_callback_(value_set_callback) {
            if (curve != nullptr) {
                curve_ = curve;
            } else {
                curve_ = TaroCurves::EASE;
            }
        }

        virtual void onNormalizedTimestampChanged(float normalized, bool reverse) {
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

        void calculate(float time) {
            if (TaroUtils::nearEqual(time, 0.0f)) {
                current_value_ = begin_;
                return;
            }
            if (TaroUtils::nearEqual(time, 1.0)) {
                current_value_ = end_;
                return;
            }
            current_value_ = evaluator_->evaluate(begin_, end_, curve_->move(time));
        }

        virtual void setSysValue() {
            current_value_ = sys_value_;
            if (value_set_callback_ != nullptr) {
                value_set_callback_(current_value_);
            }
        }

        virtual void setStartValue(bool is_reverse) {
            if (is_reverse) {
                current_value_ = end_;
            } else {
                current_value_ = begin_;
            }
        }

        private:
        T current_value_;
        T sys_value_;
        T begin_;
        T end_;
        std::shared_ptr<TaroCurve> curve_ = TaroCurves::EASE;
        ValueSetFun value_set_callback_ = nullptr;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
