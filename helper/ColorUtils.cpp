/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./ColorUtils.h"

#include "./StringUtils.h"
namespace TaroHelper {

TaroHelper::Optional<uint32_t> ColorUtils::getInt32FromNapiGetter(
    TaroRuntime::NapiGetter& getter) {
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
