/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <list>
#include <unordered_set>
#include <arkui/native_node.h>
#include <napi/native_api.h>

#include "../event_generator.h"
#include "gesture_event_base.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class EventGenerator_Gesture final : public EventGenerator {
    struct NodeListener {
        GestureOperatorPtr gesture_op_ = nullptr;
        std::unordered_set<std::string> js_event_types_;
        int event_type_ = -1;
        // bool call_js = true;
        ArkUI_NodeHandle node_handler_ = nullptr;
    };
    using NodeListenerPtr = std::shared_ptr<NodeListener>;

    public:
    explicit EventGenerator_Gesture(std::shared_ptr<TaroElement> node);
    ~EventGenerator_Gesture() = default;
    int registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) override;
    int unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle = nullptr) override;
    void gestureReceiver(ArkUI_NodeHandle node_handle, int event_type, ArkUI_GestureEvent* event);
    static void gestureReceiver(TaroGestureType event_type, ArkUI_GestureEvent* event, void* extraParams);

    private:
    NodeListenerPtr getNodeListener(ArkUI_NodeHandle node_handle, int event_type);

    private:
    std::list<NodeListenerPtr> node_listeners_;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
