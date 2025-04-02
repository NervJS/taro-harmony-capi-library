/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_NATIVEGESTUREAPI_H
#define HARMONY_LIBRARY_NATIVEGESTUREAPI_H

#include <arkui/native_gesture.h>

namespace TaroRuntime {
class NativeGestureApi {
    public:
    static ArkUI_NativeGestureAPI_1* GetInstance();

    private:
    NativeGestureApi() {}
    static ArkUI_NativeGestureAPI_1* gestureAPI;
};

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_NATIVEGESTUREAPI_H
