/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include "runtime/dom/event/taro_event.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class UserEventBase : public TaroEvent {
    public:
    UserEventBase();
    UserEventBase(const std::string& js_event_type);
    napi_value& detail();
    int serializeFun() override;

    private:
    napi_value js_detail_ = nullptr;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
