/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
