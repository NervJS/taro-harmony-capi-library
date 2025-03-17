//
// Created on 2023/12/5.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "exports/document.h"
#include "exports/event.h"
#include "exports/framework_react.h"
#include "exports/manager.h"
#include "exports/napi_bridge.h"
#include "helper/Debug.h"
#include "helper/SwitchManager.h"
#include "helper/TaroLog.h"
#include "helper/life_statistic.h"
#if IS_TEST_OPEN == 1
#include "test/index.h"
#endif

namespace TaroHarmonyLibrary {
EXTERN_C_START
// Note: 每次 import CPP 库都会调用 Init 方法
static napi_value Init(napi_env env, napi_value exports) {
    TARO_LOG_INFO("Init", "Init begins libraryName %{public}s", LIBRARY_NAME);
    if ((env == nullptr) || (exports == nullptr)) {
        TARO_LOG_WARN("Init", "env or exports is null");
        return nullptr;
    }

    std::vector<napi_property_descriptor> desc = {};

    desc.insert(desc.end(), std::begin(Manager::desc), std::end(Manager::desc));
    desc.insert(desc.end(), std::begin(Document::desc), std::end(Document::desc));
    desc.insert(desc.end(), std::begin(Event::desc), std::end(Event::desc));
    desc.insert(desc.end(), std::begin(FrameworkReact::desc), std::end(FrameworkReact::desc));
    desc.insert(desc.end(), std::begin(NapiBridge::desc), std::end(NapiBridge::desc));

#if IS_DEBUG == 1
    std::string dts = "\n";
    for (auto &item : desc) {
        if (item.utf8name != nullptr) {
            dts += "export const " + std::string(item.utf8name) + ": (...args: any[]) => any | Promise<any>\n";
        }
    }
    TARO_LOG_DEBUG("Init", "dts:%{public}s", dts.c_str());
#endif

    if (napi_define_properties(env, exports, desc.size(), desc.data()) != napi_ok) {
        TARO_LOG_WARN("Init", "napi_define_properties failed");
        return nullptr;
    }
    TaroHelper::TaroClassLifeStatistic::init(15);

    return exports;
}
EXTERN_C_END

#ifdef LIBRARY_NAME
static napi_module TaroHarmonyLibrary = {.nm_version = 1,
                                         .nm_flags = 0,
                                         .nm_filename = nullptr,
                                         .nm_register_func = Init,
                                         .nm_modname = LIBRARY_NAME,
                                         .nm_priv = ((void *)0),
                                         .reserved = {0}};

extern "C" __attribute__((constructor)) void RegisterModule(void) {
    napi_module_register(&TaroHarmonyLibrary);
}
#endif
} // namespace TaroHarmonyLibrary
