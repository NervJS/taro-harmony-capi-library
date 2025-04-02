/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./background_image.h"

#include "./types/TAngle.h"
#include "helper/ColorUtils.h"
#include "helper/StringUtils.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// https://www.w3.org/TR/css-images-3/#color-stop-syntax
static void fixStops(std::vector<float>& stops) {
    // 处理 stops 里面没有设置位置的地方
    // https://www.w3.org/TR/css-images-3/#color-stop-syntax
    int lastNumIndex = 0;
    if (std::isnan(stops[0])) stops[0] = 0;
    if (std::isnan(stops[stops.size() - 1]))
        stops[stops.size() - 1] = 1;
    for (int i = 1; i < stops.size(); i++) {
        if (std::isnan(stops[i])) continue;

        if (i - lastNumIndex > 1) {
            float step = (stops[i] - stops[lastNumIndex]) /
                         (i - lastNumIndex);
            for (int j = lastNumIndex + 1; j < i; j++) {
                stops[j] = stops[lastNumIndex] +
                           step * (j - lastNumIndex);
            }
        }
        lastNumIndex = i;
    }
    for (int i = 0; i < stops.size() - 1; i++) {
        if (stops[i] > stops[i + 1]) {
            stops[i + 1] = stops[i];
        }
    }
}

/**
 * @brief 从给定字符串视图 s 中提取第一个匹配的括号内的参数部分
 *
 * @param s 输入的字符串视图
 * @return std::string_view 括号内的内容（不包括括号本身），如果括号不匹配或没有括号，则抛出异常
 *
 * 示例:
 * std::string_view input = "function(arg1, arg2)"; 或std::string_view input = "function(arg1, arg2) xxxfdfs"; 者
 * std::string_view result = getFnParams(input);
 *  // result 应该是 "arg1, arg2"
 */
std::string_view getFnParams(std::string_view s) {
    // 找到第一个左括号 '(' 的位置
    size_t startPos = s.find("(");

    // 如果找到了左括号
    if (startPos != std::string_view::npos) {
        // 初始化左括号计数器
        int leftTimes = 0;
        // 从左括号的位置开始遍历字符串
        for (size_t i = startPos; i < s.size(); i++) {
            // 如果遇到左括号，计数器加1
            if (s[i] == '(') {
                leftTimes++;
                // 如果遇到右括号，计数器减1
            } else if (s[i] == ')') {
                leftTimes--;
                // 如果计数器归零，说明找到了匹配的右括号
                if (leftTimes == 0) {
                    // 返回括号内的内容（不包括括号本身）
                    return s.substr(startPos + 1, i - startPos - 1);
                }
            }
        }
        // 如果遍历完字符串仍未找到匹配的右括号，返回空字符串
        return "";
    } else {
        // 如果未找到左括号，返回空字符串
        return "";
    }
}

/**
 * @brief 将给定的字符串视图 s 按逗号分隔并返回各部分的字符串视图，忽略括号内的逗号。
 *
 * @param s 输入的字符串视图
 * @return std::vector<std::string_view> 分隔后的字符串视图向量
 *
 * 示例:
 * std::string_view input = "arg1, arg2, func({arg31: 1, arg32: 2}, arg4), arg5";
 * std::vector<std::string_view> result = splitParams(input);
 * // result 应该是 {"arg1", "arg2", "func({arg31: 1, arg32: 2}, arg4)", "arg5"}
 */
std::vector<std::string_view> splitParams(std::string_view s) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t level = 0;

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '{' || s[i] == '[' || s[i] == '(') {
            ++level;
        } else if (s[i] == '}' || s[i] == ']' || s[i] == ')') {
            --level;
        } else if (s[i] == ',' && level == 0) {
            result.emplace_back(string::trim(s.substr(start, i - start)));
            start = i + 1;
        }
    }

    // 添加最后一个部分
    if (start < s.size()) {
        result.emplace_back(string::trim(s.substr(start)));
    }

    return result;
}

static std::unordered_map<std::string_view,
                          ArkUI_LinearGradientDirection>
    DirectionMapping = {
        {
            "to left", /** From right to left. */
            ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT,
        },
        {
            "to top",
            /** From bottom to top. */
            ARKUI_LINEAR_GRADIENT_DIRECTION_TOP,
        },
        {
            "to right" /** From left to right. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_RIGHT,
        },
        {
            "to bottom" /** From top to bottom. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_BOTTOM,
        },
        {
            "to top left" /** From lower right to upper left. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT_TOP,
        },
        {
            "to left top" /** From lower right to upper left. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT_TOP,
        },

        {
            "to bottom left" /** From upper right to lower left. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT_BOTTOM,
        },
        {
            "to left bottom" /** From upper right to lower left. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_LEFT_BOTTOM,
        },

        {
            "to top right" /** From lower left to upper right. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_RIGHT_TOP,
        },
        {
            "to right top" /** From lower left to upper right. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_RIGHT_TOP,
        },

        {
            "to bottom right" /** From upper left to lower right. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_RIGHT_BOTTOM,
        },
        {
            "to right bottom" /** From upper left to lower right. */,
            ARKUI_LINEAR_GRADIENT_DIRECTION_RIGHT_BOTTOM,
        }};

void BackgroundImage::setValueFromNapi(const napi_value& napiValue) {
    NapiGetter getter(napiValue);
    napi_valuetype type;
    getter.GetType(type);
    if (type == napi_object) {
        auto colorsNapiGetter = getter.GetProperty("colors");
        auto colors = colorsNapiGetter.Vector();
        auto repeatingNapiGetter = getter.GetProperty("repeating");
        napi_valuetype repeatingType = repeatingNapiGetter.GetType();

        // 如果 color 有值，说明一定是渐变，先把两个渐变的共同参数解析出来
        if (colors.has_value()) {
            // 从颜色结点数据里解析出来color，stop以及对应的长度
            std::vector<uint32_t> colorArr;
            std::vector<float> stopArr;
            std::for_each(
                colors.value().begin(), colors.value().end(), [&](NapiGetter elem) {
                    auto colorValue = elem.GetProperty("0");
                    auto stopValue = elem.GetProperty("1").Double();
                    auto colorOption =
                        TaroHelper::ColorUtils::getInt32FromNapiGetter(colorValue);

                    if (stopValue.has_value() && colorOption.has_value()) {
                        colorArr.push_back(colorOption.value());
                        stopArr.push_back(stopValue.value());
                    }
                });
            int size = static_cast<int>(colorArr.size());
            if (size) {
                item_.colors = std::move(colorArr);
                item_.stops = std::move(stopArr);
                item_.size = size;
            }

            auto centerNapiGetter = getter.GetProperty("center");
            napi_valuetype centerType = centerNapiGetter.GetType();
            // 径向渐变专有参数解析
            if (centerType == napi_object) {
                auto centerX = centerNapiGetter.GetProperty("0").Double();
                auto centerY = centerNapiGetter.GetProperty("1").Double();

                item_.centerX = centerX.has_value() ? centerX.value() : 0;
                item_.centerY = centerY.has_value() ? centerY.value() : 0;

                // radialGradient
                item_.type = RADIALGRADIENT;

            } else {
                // 线性渐变专有参数解析
                auto directionNapiGetter = getter.GetProperty("direction");
                napi_valuetype directionType;
                directionNapiGetter.GetType(directionType);

                auto angleNapiGetter = getter.GetProperty("angle");
                napi_valuetype angleType;
                angleNapiGetter.GetType(angleType);
                if (directionType == napi_number) {
                    auto directValue = directionNapiGetter.Int32();
                    if (directValue.has_value()) {
                        item_.direction.set(static_cast<ArkUI_LinearGradientDirection>(
                            directValue.value()));
                    } else {
                        item_.angle = 180;
                    }
                } else if (angleType == napi_number) {
                    auto angleValue = angleNapiGetter.Double();
                    if (angleValue.has_value()) {
                        item_.angle = angleValue.value();
                    }
                } else {
                    item_.angle = 180;
                }

                item_.type = LINEARGRADIENT;
            }

            this->set(item_);
        } else {
            // 普通照片src
            auto getterValue = getter.GetProperty("src").String();
            if (getterValue.has_value()) {
                item_.type = PIC;
                item_.src = getterValue.value();
                this->set(item_);
            }
        }
    }
}

void BackgroundImage::setValueFromStringView(std::string_view value) {
    auto processColorStop = [](std::string_view param, std::vector<uint32_t>& colorArr, std::vector<float>& stopArr) {
        param = TaroHelper::string::trim(param);

        // 先尝试解析整个字符串作为颜色
        auto fullColor = TColor::MakeFromString(param).getValue();
        if (fullColor.has_value()) {
            // 如果整个字符串是一个有效的颜色值，则没有停止点
            colorArr.push_back(fullColor.value());
            stopArr.push_back(std::nanf(""));
            return;
        }

        // 检查是否以百分比结尾
        if (size_t percentPos = param.rfind('%'); percentPos != std::string_view::npos) {
            // 从百分号往前找数字的起始位置
            size_t numStart = percentPos;
            while (numStart > 0 && (std::isdigit(param[numStart - 1]) || param[numStart - 1] == '.' || param[numStart - 1] == '-')) {
                --numStart;
            }

            if (numStart > 0) {
                // 尝试解析前面的部分作为颜色
                std::string_view colorPart = TaroHelper::string::trim(param.substr(0, numStart));
                std::string_view stopPart = TaroHelper::string::trim(param.substr(numStart));

                auto c = TColor::MakeFromString(colorPart).getValue();
                if (c.has_value()) {
                    colorArr.push_back(c.value());
                    if (Dimension len = Dimension::FromString(stopPart); len.Unit() == DimensionUnit::PERCENT) {
                        stopArr.push_back(len.Value());
                    } else {
                        stopArr.push_back(std::nanf(""));
                    }
                    return;
                }
            }
        }

        // 如果上述都失败了，尝试作为纯颜色值处理
        auto c = TColor::MakeFromString(param).getValue();
        if (c.has_value()) {
            colorArr.push_back(c.value());
            stopArr.push_back(std::nanf(""));
        }
    };

    auto value2params = splitParams(value);
    if (!value2params.size()) return;
    // 因为鸿蒙只支持一个backgroundImage参数，所以先处理第一项
    value = value2params[0];
    // 如果以 url 开头，就是图片
    if (value.starts_with("url") || value.starts_with("URL")) {
        size_t endIndex = value.rfind(')');
        if (endIndex != std::string_view::npos) {
            std::string_view url = value.substr(4, endIndex - 4);
            // 去掉带引号的情况
            if (url.starts_with('"') && url.ends_with('"') || url.starts_with('\'') && url.ends_with('\'')) {
                url = url.substr(1, url.size() - 2);
            }
            if (url.size()) {
                item_.type = PIC;
                item_.src = std::string{url};
                this->set(item_);
            }
        }

    } else if (value.starts_with("linear-gradient")) { // 线性渐变
        // 取出括号里的内容
        std::string_view params = getFnParams(value);
        if (params != "") {
            // 只要params还有字符，就找逗号，取逗号前面的字符串
            std::vector<uint32_t> colorArr; // 渐变颜色数组
            std::vector<float> stopArr;     // 渐变位置数组
            item_.direction = ARKUI_LINEAR_GRADIENT_DIRECTION_BOTTOM;
            int paramNum = 0;

            auto paramVector = splitParams(params);
            for (size_t paramNum = 0; paramNum < paramVector.size(); ++paramNum) {
                auto param = paramVector[paramNum];
                if (paramNum == 0) {
                    // 如果方向的列表找得到
                    if (TAngle c = TAngle::MakeFromString(param);
                        c.unit != PropertyType::AngleUnit::UNKNOWN) {
                        item_.angle = c.getDegValue();
                    } else if (auto iter = DirectionMapping.find(param);
                               iter != DirectionMapping.end()) {
                        item_.direction = iter->second;

                    } else {
                        processColorStop(param, colorArr, stopArr);
                    }
                } else {
                    processColorStop(param, colorArr, stopArr);
                }
            }

            // 处理 stopArr 里面没有设置位置的地方
            fixStops(stopArr);

            item_.type = LINEARGRADIENT;
            item_.size = colorArr.size();
            item_.colors = std::move(colorArr);
            item_.stops = std::move(stopArr);
            this->set(item_);
        }

    } else if (value.starts_with("radial-gradient")) {
        auto processPosition = [](std::string_view param, float& x, float& y) {
            if (
                size_t start = param.find("at");
                start != std::string_view::npos) {
                std::string_view positionStr = TaroHelper::string::trim(param.substr(start + 3));

                size_t splitPos = positionStr.find(" ");
                std::string_view xStr;
                std::string_view yStr;
                if (splitPos != std::string_view::npos) {
                    xStr = param.substr(0, splitPos);
                    yStr = param.substr(splitPos + 1);
                } else {
                    xStr = param;
                    yStr = "";
                }

                // TODO 鸿蒙通过尺寸值来设置径向渐变的尺寸，但是这个位置不好拿元素的宽高，先写个壳，理论上百分比也得转成尺寸
                if (xStr.size()) {

                    Dimension xLen = Dimension::FromString(xStr);
                    switch (xLen.Unit()) {
                        case DimensionUnit::PERCENT:
                            x = xLen.Value();
                        default:
                            x = xLen.Value();
                    }
                } else {
                    x = 0.5;
                }

                if (yStr.size()) {
                    Dimension yLen = Dimension::FromString(yStr);
                    switch (yLen.Unit()) {
                        case DimensionUnit::PERCENT:
                            y = yLen.Value();
                        default:
                            y = yLen.Value();
                    }
                } else {
                    y = 0.5;
                }

                return true;
            } else {
                return false;
            }
        };

        std::string_view params = getFnParams(value);
        if (params != "") {
            std::vector<uint32_t> colorArr; // 渐变颜色数组
            std::vector<float> stopArr;     // 渐变位置数组
            float x = 0;
            float y = 0;
            auto paramVector = splitParams(params);
            for (size_t paramNum = 0; paramNum < paramVector.size(); ++paramNum) {
                auto param = paramVector[paramNum];

                if (paramNum == 0) {
                    if (!processPosition(param, x, y)) {
                        processColorStop(param, colorArr, stopArr);
                    }
                } else {
                    processColorStop(param, colorArr, stopArr);
                }
            }

            // 处理 stopArr 里面没有设置位置的地方
            fixStops(stopArr);

            item_.type = RADIALGRADIENT;
            item_.size = colorArr.size();
            item_.colors = std::move(colorArr);
            item_.stops = std::move(stopArr);
            item_.centerX = x;
            item_.centerY = y;
            this->set(item_);
        }
    }
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
