/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
