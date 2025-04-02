/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "gesture_event_pan.h"

#include "event_generator_gesture.h"
#include "gesture_factory.h"
#include "gesture_param.h"
#include "runtime/NativeGestureApi.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
void GestureEventPan::serialize() {
    TARO_LOG_DEBUG("TaroGesture", "GestureEventLongPress::serialize");
    NapiSetter::SetProperty(js_event_obj_, "type", js_event_type_);
}

GestureOperator_Pan::GestureOperator_Pan()
    : GestureOperator(TaroGestureType::Pan) {
}

int GestureOperator_Pan::addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) {
    node_handle_ = node_handle;
    GestureParam_Pan* pan_param = dynamic_cast<GestureParam_Pan*>(param);
    int32_t fingers_num = 1;
    ArkUI_GestureDirectionMask directions = GESTURE_DIRECTION_ALL;
    int32_t distance_num = 5;
    if (pan_param != nullptr) {
        fingers_num = pan_param->fingers_num;
        directions = pan_param->directions;
        distance_num = pan_param->distance_num;
    }

    recognizer_ = NativeGestureApi::GetInstance()->createPanGesture(
        fingers_num, directions, distance_num);
    if (recognizer_ == nullptr) {
        TARO_LOG_ERROR("TaroGesture", "createPanGesture failed");
        return -3;
    }
    NativeGestureApi::GetInstance()->setGestureEventTarget(recognizer_,
                                                           GESTURE_EVENT_ACTION_ACCEPT | GESTURE_EVENT_ACTION_UPDATE | GESTURE_EVENT_ACTION_END,
                                                           recall_param, [](ArkUI_GestureEvent* event, void* extraParams) {
                                                               EventGenerator_Gesture::gestureReceiver(TaroGestureType::Pan, event, extraParams);
                                                           });

    NativeGestureApi::GetInstance()->addGestureToNode(node_handle_, recognizer_, NORMAL, NORMAL_GESTURE_MASK);
    return 0;
}

int GestureOperator_Pan::removeGesture() {
    if (recognizer_) {
        NativeGestureApi::GetInstance()->removeGestureFromNode(node_handle_, recognizer_);
        NativeGestureApi::GetInstance()->dispose(recognizer_);
        recognizer_ = nullptr;
    }
    return 0;
}

REGISTER_GESTURE_EVENT(Pan, GestureEventPan, GestureOperator_Pan);

} // namespace TaroRuntime::TaroDOM::TaroEvent
