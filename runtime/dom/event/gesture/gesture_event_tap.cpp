//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
// 参考文档：https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/native__gesture_8h-V5 (OH_ArkUI_GestureEvent_GetRawInputEvent)
// 参考文档：https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/ui__input__event_8h-V5 (OH_ArkUI_PointerEvent_xxx)

#include "gesture_event_tap.h"

#include <arkui/native_gesture.h>

#include "event_generator_gesture.h"
#include "gesture_factory.h"
#include "helper/TaroLog.h"
#include "runtime/NativeGestureApi.h"
#include "runtime/dom/event/gesture/gesture_param.h"
#include "system/system_info.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
void GestureEventTap::serialize() {
    TARO_LOG_DEBUG("TaroGesture", "GestureEventTap::serialize");
    NapiSetter::SetProperty(js_event_obj_, "type", js_event_type_);
    auto inputEvent = OH_ArkUI_GestureEvent_GetRawInputEvent(event_);

    float clientX = OH_ArkUI_PointerEvent_GetX(inputEvent); // 接触点相对于当前组件左上角的X坐标。(px)
    float clientY = OH_ArkUI_PointerEvent_GetY(inputEvent); // 接触点相对于当前组件左上角的Y坐标。(px)

    float pageX = OH_ArkUI_PointerEvent_GetWindowX(inputEvent); // 接触点相对于当前应用窗口左上角的X坐标。(px)
    float pageY = OH_ArkUI_PointerEvent_GetWindowY(inputEvent); // 接触点相对于当前应用窗口左上角的Y坐标。(px)

    float screenX = OH_ArkUI_PointerEvent_GetDisplayX(inputEvent); // 接触点相对于当前屏幕左上角的X坐标。(px)
    float screenY = OH_ArkUI_PointerEvent_GetDisplayY(inputEvent); // 接触点相对于当前屏幕左上角的Y坐标。(px)

    // changedTouches填充
    napi_value napi_changed_touches;
    napi_create_array(NativeNodeApi::env, &napi_changed_touches);

    napi_value napi_changed_touches_elem;
    napi_create_object(NativeNodeApi::env, &napi_changed_touches_elem);
    NapiSetter::SetProperty(napi_changed_touches_elem, "clientX", clientX);
    NapiSetter::SetProperty(napi_changed_touches_elem, "clientY", clientY);

    NapiSetter::SetProperty(napi_changed_touches_elem, "pageX", pageX);
    NapiSetter::SetProperty(napi_changed_touches_elem, "pageY", pageY);

    NapiSetter::SetProperty(napi_changed_touches_elem, "screenX", screenX);
    NapiSetter::SetProperty(napi_changed_touches_elem, "screenY", screenY);

    // 触摸点的唯一标识符。
    NapiSetter::SetProperty(napi_changed_touches_elem, "identifier", 0);
    napi_set_element(NativeNodeApi::env, napi_changed_touches, 0, napi_changed_touches_elem);
    NapiSetter::SetProperty(js_event_obj_, "changedTouches", napi_changed_touches);

    // detail填充
    napi_value js_detail;
    napi_create_object(NativeNodeApi::env, &js_detail);
    // 点击事件的detail 带有的 x, y 同 pageX, pageY 代表距离文档左上角的距离
    NapiSetter::SetProperty(js_detail, "x", pageX);
    NapiSetter::SetProperty(js_detail, "y", pageY);
    NapiSetter::SetProperty(js_event_obj_, "detail", js_detail);
}

GestureOperator_Tap::GestureOperator_Tap()
    : GestureOperator(TaroGestureType::Tap) {}

int GestureOperator_Tap::addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) {
    node_handle_ = node_handle;
    GestureParam_Tap* press_param = dynamic_cast<GestureParam_Tap*>(param);

    int32_t count_num = 1;
    int32_t fingers_num = 1;
    if (press_param != nullptr) {
        count_num = press_param->count_num;
        fingers_num = press_param->fingers_num;
    }

    // FIXME: 临时修改，Beta.6版本会crash，Beta.5版本不会，先退回beta.5的逻辑，等beta.7发布后续再调整
    const auto device_info = TaroSystem::SystemInfo::instance()->device();
    if (TaroSystem::SystemInfo::instance()->isLessVersion(5, 0, 0, 65)) {
        recognizer_ = NativeGestureApi::GetInstance()->createTapGesture(count_num, fingers_num);
    } else {
        recognizer_ = NativeGestureApi::GetInstance()->createTapGestureWithDistanceThreshold(count_num, fingers_num, 5);
    }
    
    if (recognizer_ == nullptr) {
        TARO_LOG_ERROR("TaroGesture", "createTapGesture failed");
        return -3;
    }
    NativeGestureApi::GetInstance()->setGestureEventTarget(recognizer_,
                                                           GESTURE_EVENT_ACTION_ACCEPT,
                                                           recall_param, [](ArkUI_GestureEvent* event, void* extraParams) {
                                                               EventGenerator_Gesture::gestureReceiver(TaroGestureType::Tap, event, extraParams);
                                                           });

    NativeGestureApi::GetInstance()->addGestureToNode(node_handle_, recognizer_, NORMAL, NORMAL_GESTURE_MASK);
    return 0;
}

int GestureOperator_Tap::removeGesture() {
    if (recognizer_) {
        NativeGestureApi::GetInstance()->removeGestureFromNode(node_handle_, recognizer_);
        NativeGestureApi::GetInstance()->dispose(recognizer_);
        recognizer_ = nullptr;
    }
    return 0;
}
REGISTER_GESTURE_EVENT(Tap, GestureEventTap, GestureOperator_Tap);
} // namespace TaroRuntime::TaroDOM::TaroEvent