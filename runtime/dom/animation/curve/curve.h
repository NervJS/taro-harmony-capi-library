/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <string>

#include "helper/TaroLog.h"
#include "helper/life_statistic.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroCurve : public TaroHelper::TaroClassLife<TaroCurve> {
        public:
        TaroCurve() = default;
        virtual ~TaroCurve() = default;

        // Returns the value at specific time.
        // Notice: The value of time must be between 0.0 and 1.0 since the curve uses
        // normalized timestamp.
        float move(float time) {
            return moveInternal(time);
        }

        // Each subclass needs to override this method to implement motion in the 0.0
        // to 1.0 time range.
        virtual float moveInternal(float time) = 0;

        virtual const std::string ToString() {
            return "";
        }

        virtual bool isEqual(const std::shared_ptr<TaroCurve>& curve) const {
            return false;
        }
    };
    using TaroCurvePtr = std::shared_ptr<TaroCurve>;

    class TaroLinearCurve final : public TaroCurve {
        public:
        TaroLinearCurve() = default;
        ~TaroLinearCurve() = default;

        float moveInternal(float time) override {
            if (time < fraction_min_ || time > fraction_max_) {
                TARO_LOG_ERROR("ToraCurve",
                               "time is less than 0 or larger than 1, return 1");
                return fraction_max_;
            }
            return time;
        }

        const std::string ToString() override {
            return "linear";
        }

        private:
        float fraction_min_ = 0.0f;
        float fraction_max_ = 1.0f;
    };

    enum class TaroStepsCurvePosition {
        START,
        END,
    };

    class TaroStepsCurve final : public TaroCurve {
        public:
        explicit TaroStepsCurve(int32_t steps, TaroStepsCurvePosition position =
                                                   TaroStepsCurvePosition::START)
            : steps_(steps <= 0 ? 1 : steps),
              position_(position) {}
        ~TaroStepsCurve() = default;

        float moveInternal(float time) override {
            if (time < fraction_min || time > fraction_max) {
                TARO_LOG_ERROR("TaroCurve",
                               " time is less than 0 or larger than 1, return 1");
                return fraction_max;
            }
            auto currentStep = static_cast<int32_t>(time * steps_);
            if (position_ == TaroStepsCurvePosition::START && currentStep < steps_) {
                currentStep++;
            }
            return static_cast<float>(currentStep) / steps_;
        }

        private:
        int32_t steps_;
        const TaroStepsCurvePosition position_;
        float fraction_min = 0.0f;
        float fraction_max = 1.0f;
    };

} // namespace TaroAnimate
} // namespace TaroRuntime
