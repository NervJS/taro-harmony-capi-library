//
// Created on 2024/5/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
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
