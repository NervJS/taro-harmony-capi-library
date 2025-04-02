/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "evaluator_color.h"

#include "helper/TaroLog.h"

namespace TaroRuntime {
namespace TaroAnimate {
    double TaroEvaluatorColor::evaluate(const double& begin, const double& end,
                                        float fraction) {
        // convert begin color from ARGB to linear
        double begin_red = 0.0f;
        double begin_green = 0.0f;
        double begin_blue = 0.0f;
        double begin_alpha = 0.0f;
        uint32_t begin_uint = static_cast<uint32_t>(begin < 0.0f ? 0.0f : begin);
        uint32_t end_uint = static_cast<uint32_t>(end < 0.0f ? 0 : end);
        convertUIntToRgb(begin_uint, begin_red, begin_green, begin_blue, begin_alpha);

        // convert end color from ARGB to linear
        double end_red = 0.0f;
        double end_green = 0.0f;
        double end_blue = 0.0f;
        double end_alpha = 0.0f;
        convertUIntToRgb(end_uint, end_red, end_green, end_blue, end_alpha);

        // evaluate the result
        double red = begin_red + (end_red - begin_red) * fraction;
        double green = begin_green + (end_green - begin_green) * fraction;
        double blue = begin_blue + (end_blue - begin_blue) * fraction;
        double alpha = begin_alpha + (end_alpha - begin_alpha) * fraction;

        uint32_t rgb = 0;
        convertRgbToUInt(red, green, blue, alpha, rgb);
        // TARO_LOG_DEBUG("TaroAnimation", "begin:%{public}u(%{public}f/%{public}f/%{public}f/%{public}f) \
    //     end:%{public}u(%{public}f/%{public}f/%{public}f/%{public}f \
    //     %{public}f-%{public}u(%{public}f/%{public}f/%{public}f/%{public}f))",
        //     begin_uint, begin_red, begin_green, begin_blue, begin_alpha,
        //     end_uint, end_red, end_green, end_blue, end_alpha,
        //     fraction, rgb, red, green, blue, alpha);
        return rgb;
        ;
    }

    AnimPropType_MultiValue TaroEvaluatorColor::evaluate(
        const AnimPropType_MultiValue& begin, const AnimPropType_MultiValue& end,
        float fraction) {
        AnimPropType_MultiValue result;
        auto min_size = std::min(begin.values_.size(), end.values_.size());
        result.values_.resize(min_size);
        for (size_t idx = 0; idx < min_size; idx++) {
            if (begin.values_[idx].has_value() && end.values_[idx].has_value()) {
                double begin_value = begin.values_[idx].value();
                double end_value = end.values_[idx].value();
                result.values_[idx] = evaluate(begin_value, end_value, fraction);
            }
        }
        return std::move(result);
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
