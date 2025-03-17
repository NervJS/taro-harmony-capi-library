#pragma once
#include <arkui/native_node.h>
#include <napi/native_api.h>

#include "runtime/dom/event/event_generator.h"

namespace TaroRuntime::TaroDOM::TaroEvent {

class EventGenerator_HM final : public EventGenerator {
    struct TaroEventTarget {
        int event_type_;
        bool with_native_bubble_ = false;
        std::string js_event_type_;
    };
    struct NodeListener {
        NodeListener(ArkUI_NodeHandle node_handler)
            : node_handler_(node_handler) {}
        ArkUI_NodeHandle node_handler_ = nullptr;
        std::unordered_map<ArkUI_NodeEventType, std::vector<TaroEventTarget>> event_listeners_;
    };
    using NodeListenerPtr = std::shared_ptr<NodeListener>;

    public:
    explicit EventGenerator_HM(std::shared_ptr<TaroElement> node);
    ~EventGenerator_HM();
    int registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) override;
    int unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle) override;
    void dispatchEvent(ArkUI_NodeEvent* event);

    private:
    NodeListenerPtr getNodeListener(ArkUI_NodeHandle node_handle);

    private:
    std::unordered_map<ArkUI_NodeHandle, NodeListenerPtr> node_listeners_;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
