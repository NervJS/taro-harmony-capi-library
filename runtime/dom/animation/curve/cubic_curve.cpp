/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "cubic_curve.h"

#include "../animator/utils.h"
#include "helper/TaroLog.h"

namespace TaroRuntime {
namespace TaroAnimate {

    constexpr float FRACTION_PARAMETER_MAX = 1.0f;
    constexpr float FRACTION_PARAMETER_MIN = 0.0f;

    TaroCubicCurve::TaroCubicCurve(float x0, float y0, float x1, float y1)
        : x0_(x0), y0_(y0), x1_(x1), y1_(y1) {}

    float TaroCubicCurve::moveInternal(float time) {
        if (std::isnan(time)) {
            TARO_LOG_WARN("TaroCurve", "ToraCubicCurve time is nan, return 1");
            return FRACTION_PARAMETER_MAX;
        }
        if (time < FRACTION_PARAMETER_MIN || time > FRACTION_PARAMETER_MAX) {
            TARO_LOG_WARN("TaroCurve", "ToraCubicCurve time is less than 0 or larger than 1, return 1");
            return FRACTION_PARAMETER_MAX;
        }
        // let P0 = (0,0), P3 = (1,1)
        float start = 0.0f;
        float end = 1.0f;
        while (true) {
            float midpoint = (start + end) / 2;
            float estimate = calculateCubic(x0_, x1_, midpoint);
            if (TaroUtils::nearEqual(time, estimate, cubic_error_bound_)) {
                return calculateCubic(y0_, y1_, midpoint);
            }

            if (estimate < time) {
                start = midpoint;
            } else {
                end = midpoint;
            }
        }
        return FRACTION_PARAMETER_MAX;
    }

    const std::string TaroCubicCurve::ToString() {
        std::string curveString("cubic-bezier");
        std::string comma(",");
        curveString.append(std::string("(") + std::to_string(x0_) + comma +
                           std::to_string(y0_) + comma + std::to_string(x1_) + comma +
                           std::to_string(y1_) + std::string(")"));
        return curveString;
    }

    float TaroCubicCurve::calculateCubic(float a, float b, float m) {
        return 3.0f * a * (1.0f - m) * (1.0f - m) * m +
               3.0f * b * (1.0f - m) * m * m + m * m * m;
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
