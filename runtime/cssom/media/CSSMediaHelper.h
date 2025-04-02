/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "CSSMediaCondition.h"
#include "CSSMediaFeature.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    class CSSMediaHelper {
        public:
        static CSSMediaItemPtr createItem(const napi_value& config);
        static CSSMediaFeaturePtr createFeature(const std::vector<NapiGetter>& arr_getter);
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime
