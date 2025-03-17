#pragma once

#include "cubic_curve.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroCurves final {
        public:
        TaroCurves() = default;
        ~TaroCurves() = default;

        static const std::shared_ptr<TaroLinearCurve> LINEAR;
        static const std::shared_ptr<TaroCubicCurve> EASE;
        static const std::shared_ptr<TaroCubicCurve> EASE_IN;
        static const std::shared_ptr<TaroCubicCurve> EASE_OUT;
        static const std::shared_ptr<TaroCubicCurve> EASE_IN_OUT;
        static const std::shared_ptr<TaroStepsCurve> STEP_START;
        static const std::shared_ptr<TaroStepsCurve> STEP_END;

        static std::shared_ptr<TaroCurve> getCurve(const std::string& curve_expr);

        private:
        static bool parseCurveParam(const std::string& curve_expr,
                                    std::string& curve_name,
                                    std::vector<std::string>& curve_params);
        static std::shared_ptr<TaroCurve> createBezier(
            std::vector<std::string> curve_params);
        static std::shared_ptr<TaroCurve> createSteps(
            std::vector<std::string> curve_params);
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
