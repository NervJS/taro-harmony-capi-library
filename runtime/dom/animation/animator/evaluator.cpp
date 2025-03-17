#include "evaluator.h"

#include "runtime/cssom/stylesheet/transform.h"
namespace TaroRuntime {
namespace TaroAnimate {

    double TaroEvaluator::evaluate(const double &begin, const double &end,
                                   float fraction) {
        return begin + (end - begin) * fraction;
    }

    TaroCSSOM::TaroStylesheet::Color TaroEvaluator::evaluate(
        const TaroCSSOM::TaroStylesheet::Color &begin,
        const TaroCSSOM::TaroStylesheet::Color &end, float fraction) {
        // convert begin color from ARGB to linear
        double begin_red = 0.0f;
        double begin_green = 0.0f;
        double begin_blue = 0.0f;
        double begin_alpha = 0.0f;
        convertUIntToRgb(begin.value_or(0), begin_red, begin_green, begin_blue,
                         begin_alpha);

        // convert end color from ARGB to linear
        double end_red = 0.0f;
        double end_green = 0.0f;
        double end_blue = 0.0f;
        double end_alpha = 0.0f;
        convertUIntToRgb(end.value_or(0), end_red, end_green, end_blue, end_alpha);

        // evaluate the result
        double red = begin_red + (end_red - begin_red) * fraction;
        double green = begin_green + (end_green - begin_green) * fraction;
        double blue = begin_blue + (end_blue - begin_blue) * fraction;
        double alpha = begin_alpha + (end_alpha - begin_alpha) * fraction;

        uint32_t rgb = 0;
        convertRgbToUInt(red, green, blue, alpha, rgb);
        TaroCSSOM::TaroStylesheet::Color color;
        color.set(rgb);
        return std::move(color);
    }

    TaroCSSOM::TaroStylesheet::Transform TaroEvaluator::evaluate(
        const TaroCSSOM::TaroStylesheet::Transform &begin,
        const TaroCSSOM::TaroStylesheet::Transform &end, float fraction) {
        TaroCSSOM::TaroStylesheet::Transform res;
        res.set(TaroCSSOM::TaroStylesheet::TransformParam::staticBlend(
            begin.value(), end.value(), fraction));
        return res;
    }

    std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>
    TaroEvaluator::evaluate(
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam> &begin,
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam> &end,
        float fraction) {
        TaroCSSOM::TaroStylesheet::TransformParam res =
            TaroCSSOM::TaroStylesheet::TransformParam::staticBlend(*begin, *end,
                                                                   fraction);

        return std::make_shared<TaroCSSOM::TaroStylesheet::TransformParam>(res);
    }

    AnimPropType_MultiValue TaroEvaluator::evaluate(
        const AnimPropType_MultiValue &begin, const AnimPropType_MultiValue &end,
        float fraction) {
        AnimPropType_MultiValue result;
        auto min_size = std::min(begin.values_.size(), end.values_.size());
        result.values_.resize(min_size);
        for (size_t idx = 0; idx < min_size; idx++) {
            if (begin.values_[idx].has_value() && end.values_[idx].has_value()) {
                double begin_value = begin.values_[idx].value();
                double end_value = end.values_[idx].value();
                result.values_[idx] = begin_value + (end_value - begin_value) * fraction;
            }
        }
        return std::move(result);
    }

    Dimension TaroEvaluator::evaluate(
        const Dimension &begin,
        const Dimension &end,
        float fraction) {
        if (begin.Unit() != end.Unit()) {
            return end;
        }

        Dimension ret_value;
        ret_value.SetUnit(end.Unit());
        ret_value.SetValue(begin.Value() + (end.Value() - begin.Value()) * fraction);
        return std::move(ret_value);
    }

    void TaroEvaluator::convertUIntToRgb(uint32_t argb, double &red, double &green,
                                         double &blue, double &alpha) {
        const double GAMMA_FACTOR = 2.2;
        uint8_t u_alpha = (argb >> 24) & 0xFF;
        alpha = u_alpha;
        uint8_t u_red = (argb >> 16) & 0xFF;
        red = std::pow(u_red, GAMMA_FACTOR);
        uint8_t u_green = (argb >> 8) & 0xFF;
        green = std::pow(u_green, GAMMA_FACTOR);
        uint8_t u_blue = argb & 0xFF;
        blue = std::pow(u_blue, GAMMA_FACTOR);
    }
    void TaroEvaluator::convertRgbToUInt(double red, double green, double blue,
                                         double alpha, uint32_t &argb) {
        const double GAMMA_FACTOR = 2.2;
        uint32_t u_alpha = std::clamp(static_cast<int32_t>(alpha), 0, UINT8_MAX);
        uint32_t u_red = std::clamp(
            static_cast<int32_t>(std::pow(red, 1.0 / GAMMA_FACTOR)), 0, UINT8_MAX);
        uint32_t u_green = std::clamp(
            static_cast<int32_t>(std::pow(green, 1.0 / GAMMA_FACTOR)), 0, UINT8_MAX);
        uint32_t u_blue = std::clamp(
            static_cast<int32_t>(std::pow(blue, 1.0 / GAMMA_FACTOR)), 0, UINT8_MAX);
        argb = (u_alpha << 24) | (u_red << 16) | (u_green << 8) | u_blue;
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
