/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <napi/native_api.h>

#include "gesture_event_base.h"
#include "runtime/NapiSetter.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class GestureEventPinch final : public GestureEventBase {
    public:
    GestureEventPinch() = default;
    ~GestureEventPinch() = default;
    void serialize() override;
};

class GestureOperator_Pinch final : public GestureOperator {
    public:
    GestureOperator_Pinch();
    int addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) override;
    int removeGesture() override;
};

} // namespace TaroRuntime::TaroDOM::TaroEvent
