/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_COLOR_UTILS_H
#define TARO_CAPI_HARMONY_DEMO_COLOR_UTILS_H

#include "../runtime/NapiGetter.h"
#include "helper/Optional.h"

namespace TaroHelper {
class ColorUtils {
    public:
    static TaroHelper::Optional<uint32_t> getInt32FromNapiGetter(
        TaroRuntime::NapiGetter& getter);
};
} // namespace TaroHelper
#endif // TARO_CAPI_HARMONY_DEMO_COLOR_UTILS_H
