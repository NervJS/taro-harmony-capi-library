//
// Created on 2024/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

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
