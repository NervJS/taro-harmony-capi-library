#pragma once

#include <vector>
#include <js_native_api.h>
#include <js_native_api_types.h>

namespace TaroHarmonyLibrary {
class Event {
    public:
    static napi_value OnEventCenterOnce(napi_env env, napi_callback_info info);
    static napi_value OnEventCenter(napi_env env, napi_callback_info info);
    static napi_value OffEventCenter(napi_env env, napi_callback_info info);
    static napi_value TriggerEventCenter(napi_env env, napi_callback_info info);
    static napi_value RegisterNodeEvent(napi_env env, napi_callback_info info);
    static napi_value UnRegisterNodeEvent(napi_env env, napi_callback_info info);
    static napi_value ListenNodeEventOnce(napi_env env, napi_callback_info info);
    static napi_value TriggerNodeEvent(napi_env env, napi_callback_info info);

    public:
    static const std::vector<napi_property_descriptor> desc;

    private:
    static std::vector<std::string> parseEventNames(napi_env env, napi_value event);
};
} // namespace TaroHarmonyLibrary
