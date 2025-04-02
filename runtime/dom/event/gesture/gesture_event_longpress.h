/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <napi/native_api.h>

#include "gesture_event_base.h"
#include "runtime/NapiSetter.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class GestureEventLongPress final : public GestureEventBase {
    public:
    GestureEventLongPress() = default;
    ~GestureEventLongPress() = default;
    void serialize() override;

    public:
    int repeat_count = 0;
};

class GestureOperator_LongPress final : public GestureOperator {
    public:
    GestureOperator_LongPress();
    int addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) override;
    int removeGesture() override;
};

} // namespace TaroRuntime::TaroDOM::TaroEvent
