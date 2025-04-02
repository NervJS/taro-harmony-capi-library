/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "gesture_event_pinch.h"

#include "gesture_factory.h"
#include "gesture_param.h"
#include "runtime/NativeGestureApi.h"
#include "event_generator_gesture.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
void GestureEventPinch::serialize() {
    TARO_LOG_DEBUG("TaroGesture", "GestureEventLongPress::serialize");
    NapiSetter::SetProperty(js_event_obj_, "type", js_event_type_);
}

GestureOperator_Pinch::GestureOperator_Pinch()
    : GestureOperator(TaroGestureType::Pan) {
}

int GestureOperator_Pinch::addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) {
    node_handle_ = node_handle;
    GestureParam_Pinch* pinch_param = dynamic_cast<GestureParam_Pinch*>(param);
    int32_t fingers_num = 2;
    int32_t distance_num = 5;
    if (pinch_param != nullptr) {
        fingers_num = pinch_param->fingers_num;
        distance_num = pinch_param->distance_num;
    }

    recognizer_ = NativeGestureApi::GetInstance()->createPinchGesture(
        fingers_num, distance_num);
    if (recognizer_ == nullptr) {
        TARO_LOG_ERROR("TaroGesture", "createPinchGesture failed");
        return -3;
    }
    NativeGestureApi::GetInstance()->setGestureEventTarget(recognizer_,
                                                           GESTURE_EVENT_ACTION_ACCEPT | GESTURE_EVENT_ACTION_UPDATE | GESTURE_EVENT_ACTION_END,
                                                           recall_param, [](ArkUI_GestureEvent* event, void* extraParams) {
                                                               EventGenerator_Gesture::gestureReceiver(TaroGestureType::LongPress, event, extraParams);
                                                           });

    NativeGestureApi::GetInstance()->addGestureToNode(node_handle_, recognizer_, NORMAL, NORMAL_GESTURE_MASK);
    return 0;
}

int GestureOperator_Pinch::removeGesture() {
    if (recognizer_) {
        NativeGestureApi::GetInstance()->removeGestureFromNode(node_handle_, recognizer_);
        NativeGestureApi::GetInstance()->dispose(recognizer_);
        recognizer_ = nullptr;
    }
    return 0;
}

REGISTER_GESTURE_EVENT(Pinch, GestureEventPinch, GestureOperator_Pinch);

} // namespace TaroRuntime::TaroDOM::TaroEvent
