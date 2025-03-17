//
// Created on 2024/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "NativeGestureApi.h"

#include <arkui/native_interface.h>

namespace TaroRuntime {
ArkUI_NativeGestureAPI_1* NativeGestureApi::GetInstance() {
    static ArkUI_NativeGestureAPI_1* INSTANCE = nullptr;
    if (INSTANCE == nullptr) {
        OH_ArkUI_GetModuleInterface(ARKUI_NATIVE_GESTURE,
                                    ArkUI_NativeGestureAPI_1, INSTANCE);
    }
    return INSTANCE;
}

} // namespace TaroRuntime