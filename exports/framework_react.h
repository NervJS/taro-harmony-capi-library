//
// Created by zhutianjian on 24-9-20.
//

#pragma once

#include <vector>
#include <js_native_api.h>
#include <js_native_api_types.h>

namespace TaroHarmonyLibrary {
class FrameworkReact {
    static napi_value UpdatePropsByPayload(napi_env env, napi_callback_info info);

    public:
    static const std::vector<napi_property_descriptor> desc;
};
} // namespace TaroHarmonyLibrary
