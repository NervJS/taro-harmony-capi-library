/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_QJS_ENGINE_NAPI_BRIDGE_H
#define TARO_CAPI_HARMONY_DEMO_QJS_ENGINE_NAPI_BRIDGE_H

#include <vector>
#include <js_native_api.h>
#include <js_native_api_types.h>

namespace TaroHarmonyLibrary {
class NapiBridge {
    public:
    static napi_value onInit(napi_env env, napi_callback_info info);

    static napi_value initializeArkTSBridge(napi_env env, napi_callback_info info);

    static napi_value createQJSEngineInstance(napi_env env, napi_callback_info info);

    static napi_value loadScript(napi_env env, napi_callback_info info);

    static napi_value getNextTaroInstanceId(napi_env env, napi_callback_info info);

    static napi_value createTaroInstance(napi_env env, napi_callback_info info);

    static napi_value destroyTaroInstance(napi_env env, napi_callback_info info);

    static napi_value startEngineTest(napi_env env, napi_callback_info info);

    static const std::vector<napi_property_descriptor> desc;
};
} // namespace TaroHarmonyLibrary
#endif // TARO_CAPI_HARMONY_DEMO_QJS_ENGINE_NAPI_BRIDGE_H
