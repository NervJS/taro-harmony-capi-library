//
// Created on 2024/05/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./box_shadow.h"

#include <regex>

#include "./utils.h"
#include "helper/StringUtils.h"
#include "helper/string.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/types/TColor.h"
#include "runtime/cssom/dimension/dimension.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void BoxShadow::setValueFromStringView(std::string_view str) {
    std::cmatch matches;

    // x 偏移量 | y 偏移量 | 阴影模糊半径 | 阴影扩散半径（不支持）
    if (std::regex_search(str.begin(), str.end(), matches, std::regex(R"((?:-?\d+(?:\.\d+)?(?:px|vw|vh|r?em)\s*){2,4})"))) {
        size_t start = matches[0].first - str.begin();
        auto values = TaroHelper::string::splitBySpace(str.substr(start, matches[0].length()));
        auto offsetX = Dimension(values[0]).ParseToVp();
        auto offsetY = Dimension(values[1]).ParseToVp();
        if (!offsetX.has_value() || !offsetY.has_value()) return;
        boxShadowItem.offsetX.set(offsetX.value());
        boxShadowItem.offsetY.set(offsetY.value());
        if (values.size() > 2) {
            auto radius = Dimension(values[2]).ParseToVp();
            if (radius.has_value()) {
                boxShadowItem.radius.set(radius.value());
            }
        }
    } else {
        return;
    }

    // 颜色
    if (std::regex_search(str.begin(), str.end(), matches, std::regex(R"(#[0-9a-fA-F]{3,6}|(?:rgba?|hsl|hwb|lab|lch)\(.*?\)|transparent)"))) {
        size_t start = matches[0].first - str.begin();
        auto colorStr = str.substr(start, matches[0].length());
        boxShadowItem.color.set(TColor::MakeFromString(colorStr).getValue());
    }

    // 内阴影
    auto insetPos = str.find("inset");
    if (insetPos != std::string_view::npos) {
        boxShadowItem.fill.set(true);
    }

    this->set(boxShadowItem);
}

void BoxShadow::setValueFromNapi(const napi_value &napiValue) {
    NapiGetter getter(napiValue);
    napi_valuetype type;

    {
        auto radius = getter.GetProperty("radius");
        if (radius.Type() == napi_number) {
            auto getterValue = radius.Double();
            if (getterValue.has_value()) {
                boxShadowItem.radius.set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (radius.Type() == napi_string) {
            auto getterValue = radius.String();
            if (getterValue.has_value()) {
                auto num = Dimension(getterValue.value()).ParseToVp(true);
                if (num.has_value()) {
                    boxShadowItem.radius.set(num.value());
                }
            }
        }
    }
    {
        auto offsetX = getter.GetProperty("offsetX");
        if (offsetX.Type() == napi_number) {
            auto getterValue = offsetX.Double();
            if (getterValue.has_value()) {
                boxShadowItem.offsetX.set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (offsetX.Type() == napi_string) {
            auto getterValue = offsetX.String();
            if (getterValue.has_value()) {
                auto num = Dimension(getterValue.value()).ParseToVp(true);
                if (num.has_value()) {
                    boxShadowItem.offsetX.set(num.value());
                }
            }
        }
    }
    {
        auto offsetY = getter.GetProperty("offsetY");
        if (offsetY.Type() == napi_number) {
            auto getterValue = offsetY.Double();
            if (getterValue.has_value()) {
                boxShadowItem.offsetY.set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (offsetY.Type() == napi_string) {
            auto getterValue = offsetY.String();
            if (getterValue.has_value()) {
                auto num = Dimension(getterValue.value()).ParseToVp(true);
                if (num.has_value()) {
                    boxShadowItem.offsetY.set(num.value());
                }
            }
        }
    }
    {
        auto fill = getter.GetProperty("fill");
        if (fill.Type() == napi_boolean) {
            auto getterValue = fill.Bool();
            if (getterValue.has_value()) {
                boxShadowItem.fill.set(getterValue.value());
                this->set(boxShadowItem);
            }
        }
    }
    {
        auto colorGetter = getter.GetProperty("color");
        if (colorGetter.Type() == napi_number) {
            auto getterValue = colorGetter.Int32();
            if (getterValue.has_value()) {
                boxShadowItem.color.set(getterValue.value());
                this->set(boxShadowItem);
            }
        } else if (type == napi_string) {
            auto getterValue = colorGetter.String();
            if (getterValue.has_value()) {
                auto color = TaroHelper::StringUtils::parseColor(getterValue.value());
                if (color.has_value()) {
                    boxShadowItem.color.set(color.value());
                    this->set(boxShadowItem);
                }
            }
        }
    }

    this->set(boxShadowItem);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
