/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <string>
#include <arkui/native_type.h>

namespace TaroRuntime::TaroDOM {
class TaroElement;
}

namespace TaroRuntime::TaroDOM::TaroEvent {
struct EventListenParams;
enum class EventGeneratorType {
    Invalid = 0,
    Event = 1,
    UserDefine = 2,
    Gesture = 3,
};

class EventGenerator {
    public:
    EventGenerator(EventGeneratorType gen_type, std::shared_ptr<TaroElement> node);
    virtual ~EventGenerator() = default;
    virtual int registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) = 0;
    virtual int unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle) = 0;

    protected:
    EventGeneratorType gen_type_;
    std::weak_ptr<TaroElement> node_owner_;
    int32_t nid_ = 0;
};

using EventGeneratorPtr = std::shared_ptr<EventGenerator>;
} // namespace TaroRuntime::TaroDOM::TaroEvent
