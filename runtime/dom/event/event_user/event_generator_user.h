#pragma once

#include <arkui/native_node.h>
#include <napi/native_api.h>

#include "runtime/dom/event/event_generator.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class EventGenerator_User final : public EventGenerator {
    public:
    explicit EventGenerator_User(std::shared_ptr<TaroElement> node)
        : EventGenerator(EventGeneratorType::UserDefine, node) {}
    int registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) override {
        return 0;
    }
    int unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle) override {
        return 0;
    }
    int triggerEvent(napi_value n_val);
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
