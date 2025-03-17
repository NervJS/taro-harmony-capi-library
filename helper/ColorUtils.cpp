//
// Created on 2024/5/27.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
#include "./ColorUtils.h"

#include "./StringUtils.h"
namespace TaroHelper {

TaroHelper::Optional<uint32_t> ColorUtils::getInt32FromNapiGetter(
    TaroRuntime::NapiGetter &getter) {
    TaroHelper::Optional<uint32_t> res;
    napi_valuetype type;
    getter.GetType(type);
    if (type == napi_number) {
        auto getterValue = getter.UInt32();
        if (getterValue.has_value()) {
            res.set(getterValue.value());
        }
    } else if (type == napi_string) {
        auto getterValue = getter.String();
        if (getterValue.has_value()) {
            auto color = TaroHelper::StringUtils::parseColor(getterValue.value());
            if (color.has_value()) {
                res.set(color.value());
            }
        }
    }

    return res;
}
} // namespace TaroHelper
