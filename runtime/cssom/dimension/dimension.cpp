/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./dimension.h"

#include <array>
#include <functional>
#include <regex>

#include "./context.h"
#include "./string_utils.h"
#include "./utils.h"
#include "runtime/cssom/stylesheet/utils.h"

namespace TaroRuntime {

namespace {

    struct CalcDimensionParam {
        float value = 0.0f;
        float vpScale = 0.0f;
        float fpScale = 0.0f;
        float lpxScale = 0.0f;
        float parentLength = 0.0f;
    };

    using CalcDimensionFunc = std::function<bool(const CalcDimensionParam& param, double& result)>;
    bool CalcDimensionNone(const CalcDimensionParam& param, double& result) {
        result = param.value;
        return true;
    }

    bool CalcDimensionPx(const CalcDimensionParam& param, double& result) {
        result = param.value;
        return true;
    }

    bool CalcDimensionPercent(const CalcDimensionParam& param, double& result) {
        if (NonNegative(param.parentLength)) {
            result = param.value * param.parentLength;
            return true;
        }
        return false;
    }

    bool CalcDimensionVp(const CalcDimensionParam& param, double& result) {
        if (Positive(param.vpScale)) {
            result = param.value * param.vpScale;
            return true;
        }
        return false;
    }

    bool CalcDimensionFp(const CalcDimensionParam& param, double& result) {
        if (Positive(param.fpScale) && Positive(param.vpScale)) {
            result = param.value * param.fpScale * param.vpScale;
            return true;
        }
        return false;
    }

    bool CalcDimensionLpx(const CalcDimensionParam& param, double& result) {
        if (Positive(param.lpxScale)) {
            result = param.value * param.lpxScale;
            return true;
        }
        return false;
    }

    std::unordered_map<DimensionUnit, CalcDimensionFunc> calcDimensionFuncMap_ = {
        {DimensionUnit::NONE, &CalcDimensionNone},
        {DimensionUnit::PX, &CalcDimensionPx},
        {DimensionUnit::PERCENT, &CalcDimensionPercent},
        {DimensionUnit::VP, &CalcDimensionVp},
        {DimensionUnit::FP, &CalcDimensionFp},
        {DimensionUnit::LPX, &CalcDimensionLpx}};
} // namespace

double Dimension::ConvertToVp() const {
    if (unit_ == DimensionUnit::VP) {
        return value_;
    }

    DimensionContextRef dimensionContext = DimensionContext::GetCurrentContext();
    if (unit_ == DimensionUnit::NONE) {
        return value_ / dimensionContext->density_pixels_;
    }
    if (auto val = ParseToVp(dimensionContext); val.has_value()) {
        return val.value();
    }
    return 0.0;
}

double Dimension::ConvertToVp(DimensionContextRef context) const {
    if (unit_ == DimensionUnit::VP) {
        return value_;
    }

    if (unit_ == DimensionUnit::NONE) {
        return value_ / context->density_pixels_;
    }
    if (auto val = ParseToVp(context); val.has_value()) {
        return val.value();
    }
    return 0.0;
}

double Dimension::ConvertToPx() const {
    DimensionContextRef dimensionContext = DimensionContext::GetCurrentContext();
    return ConvertToPx(dimensionContext);
}

double Dimension::ConvertToPx(DimensionContextRef dimensionContext) const {
    if (unit_ == DimensionUnit::NONE) {
        return value_;
    }
    if (unit_ == DimensionUnit::PX) {
        return value_;
    }
    return ConvertToVp(dimensionContext) * dimensionContext->density_pixels_;
}

double Dimension::ConvertToPxWithSize(double size) const {
    if (unit_ == DimensionUnit::PERCENT) {
        return value_ * size;
    }
    return ConvertToPx();
}

std::string Dimension::ToString() const {
    static const int32_t unitsNum = 6;
    static const int32_t percentIndex = 3;
    static const int32_t percentUnit = 100;
    static std::array<std::string, unitsNum> units = {"px", "vp", "fp", "%", "lpx", "auto"};
    if (static_cast<int>(unit_) >= unitsNum) {
        return StringUtils::DoubleToString(value_).append("px");
    }
    if (unit_ == DimensionUnit::NONE) {
        return StringUtils::DoubleToString(value_).append("none");
    }
    if (units[static_cast<int>(unit_)] == units[percentIndex]) {
        return StringUtils::DoubleToString(value_ * percentUnit).append(units[static_cast<int>(unit_)]);
    }
    return StringUtils::DoubleToString(value_).append(units[static_cast<int>(unit_)]);
}

std::string extractEnvContent(std::string_view cssValue) {
    // 将 std::string_view 转换为 std::string 以便使用 std::regex
    std::string cssValueStr(cssValue);

    // 定义正则表达式来匹配 env() 函数及其内容
    std::regex re(R"(env\(([^)]+)\))");
    std::smatch match;

    // 检查输入字符串是否匹配正则表达式
    if (std::regex_search(cssValueStr, match, re)) {
        // 提取并返回第一个捕获组（即 env() 函数中的内容）
        return match[1].str();
    } else {
        return "";
    }
}

// for example str = 0.00px
Dimension Dimension::FromString(const std::string& str) {
    return FromString((std::string_view)str);
}

std::optional<double> Dimension::ParseToVp() const {
    return ParseToVp(DimensionContext::GetCurrentContext());
}

std::optional<double> Dimension::ParseToVp(DimensionContextRef context) const {
    std::optional<double> result;

    if (unit_ == DimensionUnit::VP) {
        result.emplace(value_);
    } else if (unit_ == DimensionUnit::PX) {
        result.emplace(px2Vp(value_));
    } else if (unit_ == DimensionUnit::VW) {
        result.emplace(value_ * context->viewport_width_ / 100);
    } else if (unit_ == DimensionUnit::VH) {
        result.emplace(value_ * context->viewport_height_ / 100);
    } else if (unit_ == DimensionUnit::DESIGN_PX) {
        result.emplace(value_ * context->design_ratio_);
    } else if (unit_ == DimensionUnit::SAFE_AREA) {
        switch (static_cast<SAFE_AREA_DIRECTION>(value_)) {
            case SAFE_AREA_DIRECTION::TOP: {
                result.emplace(context->safeArea.top);
                break;
            }
            case SAFE_AREA_DIRECTION::LEFT: {
                result.emplace(context->safeArea.left);
                break;
            }
            case SAFE_AREA_DIRECTION::BOTTOM: {
                result.emplace(context->viewport_height_ - context->safeArea.bottom);
                break;
            }
            case SAFE_AREA_DIRECTION::RIGHT: {
                result.emplace(context->viewport_width_ - context->safeArea.right);
                break;
            }
        }
    }

    return result;
}

std::optional<double> Dimension::ParseToVp(double base) const {
    if (unit_ == DimensionUnit::PERCENT) {
        return base * value_;
    } else {
        return ParseToVp();
    }
}

std::optional<double> Dimension::ParseToVp(DimensionContextRef context, double base) const {
    if (unit_ == DimensionUnit::PERCENT) {
        return base * value_;
    } else {
        return ParseToVp(context);
    }
}

bool isValidCalcString(const std::string& calcString) {
    // 定义合法的CSS单位
    std::string units = "px|%|vw|vh|vp";
    // 定义数字的正则表达式
    std::string numberRegex = "[-+]?\\d*\\.?\\d+";
    // 定义完整的calc字符串的正则表达式
    std::string calcRegex = "^calc\\(\\s*(" + numberRegex + ")\\s*(" + units + ")?\\s*([+-]\\s*(" + numberRegex + ")\\s*(" + units + ")?\\s*)*\\)$";
    // 使用正则表达式匹配calc字符串
    std::regex regex(calcRegex);
    return std::regex_match(calcString, regex);
}

const bool Dimension::CheckCalcValid() const {
    return isValidCalcString(GetCalcExpression());
}

// for example str = 0.00px
Dimension Dimension::FromString(const std::string_view& str) {
    static const int32_t percentUnit = 100;
    static const std::unordered_map<std::string, DimensionUnit> uMap{
        {"lpx", DimensionUnit::DESIGN_PX},
        {"px", DimensionUnit::PX},
        {"vp", DimensionUnit::VP},
        {"%", DimensionUnit::PERCENT},
        {"auto", DimensionUnit::AUTO},
        {"vw", DimensionUnit::VW},
        {"vh", DimensionUnit::VH},
    };

    double value = 0.0;
    DimensionUnit unit = DimensionUnit::AUTO;

    if (str.empty()) {
        return Dimension(value, unit);
    }

    if (str.size() >= 4 && str.compare(0, 4, "auto") == 0) {
        // auto
        return Dimension(value, DimensionUnit::AUTO);
    }
    if (str.size() >= 4 && str.compare(0, 4, "calc") == 0) {
        // calc()
        if (std::string expr{str}; isValidCalcString(expr)) {
            auto dimension = Dimension(value, DimensionUnit::CALC);
            dimension.SetCalcExpression(std::string(str));
            return dimension;
        } else {
            return Dimension{0, DimensionUnit::INVALID};
        }
    }
    if (str.compare(0, 3, "env") == 0) {
        // 安全区域
        auto dimension = Dimension(0, DimensionUnit::SAFE_AREA);
        auto envRes = extractEnvContent(str);
        if (envRes == "safe-area-inset-bottom") {
            dimension.SetValue(static_cast<double>(SAFE_AREA_DIRECTION::BOTTOM));
        } else if (envRes == "safe-area-inset-top") {
            dimension.SetValue(static_cast<double>(SAFE_AREA_DIRECTION::TOP));
        } else if (envRes == "safe-area-inset-left") {
            dimension.SetValue(static_cast<double>(SAFE_AREA_DIRECTION::LEFT));
        } else if (envRes == "safe-area-inset-right") {
            dimension.SetValue(static_cast<double>(SAFE_AREA_DIRECTION::RIGHT));
        }
        return dimension;
    }

    for (int32_t i = static_cast<int32_t>(str.length() - 1); i >= 0; --i) {
        if (str[i] >= '0' && str[i] <= '9') {
            value = StringUtils::StringToDouble(std::string(str.substr(0, i + 1)));
            auto subStr = std::string(str.substr(i + 1));
            // 如果没有单位，把他当成vp来看待
            if (subStr.size() == 0) {
                unit = DimensionUnit::VP;
            } else {
                auto iter = uMap.find(subStr);
                if (iter != uMap.end()) {
                    unit = iter->second;
                }
            }
            value = unit == DimensionUnit::PERCENT ? value / percentUnit : value;
            break;
        }
    }
    return Dimension(value, unit);
}

float vp2Px(const float& val) {
    return val * DimensionContext::GetCurrentContext()->density_pixels_;
}

float px2Vp(const float& val) {
    return val / DimensionContext::GetCurrentContext()->density_pixels_;
}

} // namespace TaroRuntime
