//
// Created on 2024/6/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NapiDialogNodeApi.h"
#include <arkui/native_interface.h>



namespace TaroRuntime {

NapiDialogNodeApi* NapiDialogNodeApi::instance = nullptr;

NapiDialogNodeApi* NapiDialogNodeApi::GetInstance() {
    if (instance == nullptr) {
        instance = new NapiDialogNodeApi();
        OH_ArkUI_GetModuleInterface(ARKUI_NATIVE_DIALOG, ArkUI_NativeDialogAPI_1, instance->api);
    }
    return instance;
}

ArkUI_NativeDialogAPI_1* NapiDialogNodeApi::GetAPI() {
    return api;
}

} // namespace TaroRuntime
