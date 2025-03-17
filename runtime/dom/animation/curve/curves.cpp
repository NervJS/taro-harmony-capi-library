#include "curves.h"

#include "helper/StringUtils.h"
#include "runtime/dom/animation/animator/utils.h"

namespace TaroRuntime {
namespace TaroAnimate {
    const std::shared_ptr<TaroLinearCurve> TaroCurves::LINEAR =
        std::make_shared<TaroLinearCurve>();
    const std::shared_ptr<TaroCubicCurve> TaroCurves::EASE =
        std::make_shared<TaroCubicCurve>(0.25f, 0.1f, 0.25f, 1.0f);
    const std::shared_ptr<TaroCubicCurve> TaroCurves::EASE_IN =
        std::make_shared<TaroCubicCurve>(0.42f, 0.0f, 1.0f, 1.0f);
    const std::shared_ptr<TaroCubicCurve> TaroCurves::EASE_OUT =
        std::make_shared<TaroCubicCurve>(0.0f, 0.0f, 0.58f, 1.0f);
    const std::shared_ptr<TaroCubicCurve> TaroCurves::EASE_IN_OUT =
        std::make_shared<TaroCubicCurve>(0.42f, 0.0f, 0.58f, 1.0f);
    const std::shared_ptr<TaroStepsCurve> TaroCurves::STEP_START =
        std::make_shared<TaroStepsCurve>(1, TaroStepsCurvePosition::START);
    const std::shared_ptr<TaroStepsCurve> TaroCurves::STEP_END =
        std::make_shared<TaroStepsCurve>(1, TaroStepsCurvePosition::END);

    std::shared_ptr<TaroCurve> TaroCurves::getCurve(const std::string &curve_expr) {
        static std::unordered_map<std::string, std::shared_ptr<TaroCurve> >
            map_curves = {{"ease", TaroCurves::EASE},
                          {"linear", TaroCurves::LINEAR},
                          {"ease-in", TaroCurves::EASE_IN},
                          {"ease-out", TaroCurves::EASE_OUT},
                          {"ease-in-out", TaroCurves::EASE_IN_OUT},
                          {"step-start", TaroCurves::STEP_START},
                          {"step-end", TaroCurves::STEP_END}};

        if (curve_expr.empty()) {
            TaroCurves::EASE;
        }

        auto iter = map_curves.find(curve_expr);
        // 预准备的曲线
        if (iter != map_curves.end()) {
            return iter->second;
        }

        // 其他曲线
        std::string curve_name;
        std::vector<std::string> curve_params;
        bool parse_suc = parseCurveParam(curve_expr, curve_name, curve_params);
        if (!parse_suc) {
            TARO_LOG_ERROR("TaroCurve", "parse curve param:%{public}s failed",
                           curve_expr.c_str());
            return TaroCurves::EASE;
        }

        if (curve_name == "cubic-bezier") {
            return createBezier(curve_params);
        } else if (curve_name == "steps") {
            return createSteps(curve_params);
        } else {
            TARO_LOG_ERROR("TaroCurve",
                           "curve function is not exist, curve param:%{public}s failed",
                           curve_expr.c_str());
        }

        return TaroCurves::EASE;
    }

    bool TaroCurves::parseCurveParam(const std::string &curve_expr,
                                     std::string &curve_name,
                                     std::vector<std::string> &curve_params) {
        if (curve_expr.back() != ')') {
            return false;
        }
        auto left_embrace_position = curve_expr.find_last_of('(');
        if (left_embrace_position == std::string::npos) {
            return false;
        }
        auto tim_func_name = curve_expr.substr(0, left_embrace_position);
        auto params =
            curve_expr.substr(left_embrace_position + 1,
                              curve_expr.length() - left_embrace_position - 2);
        if (tim_func_name.empty() || params.empty()) {
            return false;
        }
        TaroHelper::StringUtils::split(curve_params, params, ",");
        for (auto &param : curve_params) {
            TaroUtils::removeHeadTailSpace(param);
        }
        curve_name = std::move(tim_func_name);
        return true;
    }

    std::shared_ptr<TaroCurve> TaroCurves::createBezier(
        std::vector<std::string> curve_params) {
        if (curve_params.size() != 4) {
            return nullptr;
        }
        float x1 = TaroUtils::stringToFloat(curve_params.at(0));
        float y1 = TaroUtils::stringToFloat(curve_params.at(1));
        float x2 = TaroUtils::stringToFloat(curve_params.at(2));
        float y2 = TaroUtils::stringToFloat(curve_params.at(3));
        return std::make_shared<TaroCubicCurve>(x1, y1, x2, y2);
    }

    std::shared_ptr<TaroCurve> TaroCurves::createSteps(
        std::vector<std::string> curve_params) {
        if (curve_params.empty() || curve_params.size() > 2) {
            return nullptr;
        }
        auto step = TaroUtils::stringToInt(curve_params.front());
        if (step <= 0) {
            return nullptr;
        }
        TaroStepsCurvePosition position = TaroStepsCurvePosition::END;
        if (curve_params.size() > 1) {
            const std::string &back_str = curve_params.back();
            if (back_str == "start") {
                position = TaroStepsCurvePosition::START;
            } else if (back_str == "end") {
                position = TaroStepsCurvePosition::END;
            } else {
                return nullptr;
            }
        }
        return std::make_shared<TaroStepsCurve>(step, position);
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
