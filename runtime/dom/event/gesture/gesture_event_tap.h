//
// Created on 2024/8/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once
#include <napi/native_api.h>

#include "gesture_event_base.h"
#include "runtime/NapiSetter.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class GestureEventTap final : public GestureEventBase {
    public:
    GestureEventTap() = default;
    ~GestureEventTap() = default;
    void serialize() override;

    public:
};

class GestureOperator_Tap final : public GestureOperator{
public:
    GestureOperator_Tap();
    int addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) override ;
    int removeGesture() override ;
};

} // namespace TaroRuntime::TaroDOM::TaroEvent
