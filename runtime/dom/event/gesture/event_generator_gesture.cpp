/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_generator_gesture.h"

#include <cstdint>

#include "gesture_factory.h"
#include "runtime/dom/document.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime::TaroDOM::TaroEvent {

EventGenerator_Gesture::EventGenerator_Gesture(std::shared_ptr<TaroElement> node)
    : EventGenerator(EventGeneratorType::Gesture, node) {}

int EventGenerator_Gesture::registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) {
    auto node = node_owner_.lock();
    auto node_handle = (!param || !param->node_handle_) ? node->GetNodeHandle() : param->node_handle_;
    auto listener = getNodeListener(node_handle, event_type);
    if (listener) {
        listener->js_event_types_.insert(js_event_type);
        return 0;
    }
    auto gesture_op = TaroGestureFactory::instance()->createGestureOperator(event_type);
    if (!gesture_op) {
        TARO_LOG_ERROR("TaroGesture", "invalid type:%{public}d", event_type);
        return -1;
    }
    listener = std::make_shared<NodeListener>();
    listener->event_type_ = event_type;
    listener->js_event_types_.insert(js_event_type);
    listener->node_handler_ = node_handle;
    listener->gesture_op_ = gesture_op;
    node_listeners_.push_back(listener);
    GestureParam* gesture_param = param ? (GestureParam*)param->ext_data_ : nullptr;
    gesture_op->addGesture(node_handle, gesture_param, reinterpret_cast<void*>(node->nid_));
    return 0;
}

int EventGenerator_Gesture::unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle) {
    for (auto iter = node_listeners_.begin(); iter != node_listeners_.end();) {
        auto& tmp_listener = *iter;
        if (tmp_listener->node_handler_ == node_handle && tmp_listener->event_type_ == event_type) {
            tmp_listener->js_event_types_.erase(js_event_type);
            if (tmp_listener->js_event_types_.empty()) {
                tmp_listener->gesture_op_->removeGesture();
                iter = node_listeners_.erase(iter);
            } else {
                ++iter;
            }

            break;
        } else {
            ++iter;
        }
    }

    return 0;
}

EventGenerator_Gesture::NodeListenerPtr EventGenerator_Gesture::getNodeListener(ArkUI_NodeHandle node_handle, int event_type) {
    for (auto& listener : node_listeners_) {
        if ((node_handle == nullptr || listener->node_handler_ == node_handle) && listener->event_type_ == event_type) {
            return listener;
        }
    }
    return nullptr;
}

void EventGenerator_Gesture::gestureReceiver(ArkUI_NodeHandle node_handle, int event_type, ArkUI_GestureEvent* event) {
    auto emitter = node_owner_.lock()->getEventEmitter();
    if (emitter == nullptr) {
        return;
    }
    auto listener = getNodeListener(node_handle, event_type);
    if (listener == nullptr) {
        TARO_LOG_ERROR("TaroGesture", "listener is not found, event_type:%{public}d node:%{public}p", event_type, node_handle);
        return;
    }

    // 多一次copy，防止操作过程中发生事件释放
    auto js_event_types = listener->js_event_types_;
    for (const auto& js_event_type : js_event_types) {
        auto gesture_event = TaroGestureFactory::instance()->createGestureEvent(event_type);
        if (gesture_event == nullptr) {
            TARO_LOG_ERROR("TaroGesture", "event_type:%{public}d is not found", event_type);
            return;
        }
        gesture_event->setEvent(event);
        gesture_event->gen_type_ = EventGeneratorType::Gesture;
        gesture_event->event_type = event_type;
        gesture_event->js_event_type_ = js_event_type;
        gesture_event->node_handle_ = listener->node_handler_;
        emitter->triggerEvents(gesture_event);
    }
}

void EventGenerator_Gesture::gestureReceiver(TaroGestureType event_type, ArkUI_GestureEvent* event, void* extraParams) {
    int32_t node_id = reinterpret_cast<int64_t>(extraParams);
    auto node = TaroDocument::GetInstance()->GetElementById(node_id);
    if (!node) {
        TARO_LOG_ERROR("TaroGesture", "node:%{public}d not exist", node_id);
        return;
    }
    auto generator = dynamic_pointer_cast<EventGenerator_Gesture>(
        node->getEventEmitter()->getGenerator(EventGeneratorType::Gesture));
    generator->gestureReceiver(nullptr, (int)event_type, event);
}

} // namespace TaroRuntime::TaroDOM::TaroEvent