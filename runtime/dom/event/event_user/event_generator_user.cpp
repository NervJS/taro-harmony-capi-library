/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_generator_user.h"

#include "runtime/dom/element/element.h"
#include "runtime/dom/event/event_emitter.h"
#include "user_event.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
int EventGenerator_User::triggerEvent(napi_value n_val) {
    auto node = node_owner_.lock();
    auto user_event = std::make_shared<UserEventBase>();
    user_event->SetNapiValue(n_val);
    user_event->gen_type_ = EventGeneratorType::UserDefine;
    user_event->event_type = -1;
    user_event->node_handle_ = node->GetNodeHandle();
    user_event->jsToMem();
    node->getEventEmitter()->triggerEvents(user_event);
    return 0;
}
} // namespace TaroRuntime::TaroDOM::TaroEvent
