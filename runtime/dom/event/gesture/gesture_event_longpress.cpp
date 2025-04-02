/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "gesture_event_longpress.h"

#include "event_generator_gesture.h"
#include "gesture_factory.h"
#include "gesture_param.h"
#include "helper/TaroLog.h"
#include "runtime/NativeGestureApi.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
void GestureEventLongPress::serialize() {
    TARO_LOG_DEBUG("TaroGesture", "GestureEventLongPress::serialize");
    NapiSetter::SetProperty(js_event_obj_, "type", js_event_type_);
}

GestureOperator_LongPress::GestureOperator_LongPress()
    : GestureOperator(TaroGestureType::LongPress) {
}

int GestureOperator_LongPress::addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) {
    node_handle_ = node_handle;
    GestureParam_LongPress* press_param = dynamic_cast<GestureParam_LongPress*>(param);
    int32_t fingers_num = 1;
    bool repeat_result = false;
    int32_t duration_num = 500;
    if (press_param != nullptr) {
        fingers_num = press_param->fingers_num;
        repeat_result = press_param->repeat_result;
        duration_num = press_param->duration_num;
    }

    recognizer_ = NativeGestureApi::GetInstance()->createLongPressGesture(
        fingers_num, repeat_result, duration_num);
    if (recognizer_ == nullptr) {
        TARO_LOG_ERROR("TaroGesture", "createLongPressGesture failed");
        return -3;
    }
    NativeGestureApi::GetInstance()->setGestureEventTarget(recognizer_,
                                                           GESTURE_EVENT_ACTION_ACCEPT,
                                                           recall_param, [](ArkUI_GestureEvent* event, void* extraParams) {
                                                               EventGenerator_Gesture::gestureReceiver(TaroGestureType::LongPress, event, extraParams);
                                                           });

    NativeGestureApi::GetInstance()->addGestureToNode(node_handle_, recognizer_, NORMAL, NORMAL_GESTURE_MASK);
    return 0;
}

int GestureOperator_LongPress::removeGesture() {
    if (recognizer_) {
        NativeGestureApi::GetInstance()->removeGestureFromNode(node_handle_, recognizer_);
        NativeGestureApi::GetInstance()->dispose(recognizer_);
        recognizer_ = nullptr;
    }
    return 0;
}

REGISTER_GESTURE_EVENT(LongPress, GestureEventLongPress, GestureOperator_LongPress);

} // namespace TaroRuntime::TaroDOM::TaroEvent
