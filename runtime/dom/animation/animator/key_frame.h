/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "helper/TaroLog.h"
#include "helper/life_statistic.h"
#include "runtime/dom/animation/curve/curves.h"

namespace TaroRuntime {
namespace TaroAnimate {

    template <typename T>
    class TaroKeyframe final : public TaroHelper::TaroClassLife<TaroKeyframe<T>> {
        public:
        TaroKeyframe(float key_time, const T &key_value)
            : key_time_(key_time), key_value_(key_value) {}
        ~TaroKeyframe() = default;

        float getKeyTime() const {
            return key_time_;
        }

        void setKeyValue(const T &key_value) {
            key_value_ = key_value;
        }

        const T &getKeyValue() const {
            return key_value_;
        }

        void setCurve(const std::shared_ptr<TaroCurve> &curve) {
            if (!curve) {
                TARO_LOG_ERROR("TaroCurve", "set curve failed, input curve is null.");
                return;
            }
            curve_ = curve;
        }

        std::shared_ptr<TaroCurve> getCurve() {
            return curve_;
        }

        bool isValid() const {
            if (key_time_ < 0.0f || key_time_ > 1.0f) {
                TARO_LOG_ERROR("TaroCurve", "invalid normalized key time: %{public}f",
                               key_time_);
                return false;
            }
            return true;
        }

        private:
        const float key_time_{0.0f};
        T key_value_;
        std::shared_ptr<TaroCurve> curve_{TaroCurves::EASE};
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
