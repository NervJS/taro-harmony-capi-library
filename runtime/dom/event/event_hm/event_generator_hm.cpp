#include "event_generator_hm.h"

#include "event_adapter.h"
#include "event_factory.h"
#include "event_task.h"
#include "event_task_thread.h"
#include "helper/TaroTimer.h"
#include "helper/Time.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/event/event_emitter.h"
#include "runtime/render.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
EventGenerator_HM::EventGenerator_HM(std::shared_ptr<TaroElement> node)
    : EventGenerator(EventGeneratorType::Event, node) {}

EventGenerator_HM::~EventGenerator_HM() {
    for (const auto& node_listener : node_listeners_) {
        TaroEventAdapter::instance()->clearNodeEvent(nid_, node_listener.first);
    }
    node_listeners_.clear();
}

int EventGenerator_HM::registerEvent(const std::string& js_event_type, int event_type, EventListenParams* param) {
    // 获取事件对应的鸿蒙事件类型
    ArkUI_NodeEventType hm_event_type;
    int ret = TaroEventFactory::instance()->getHmEventType(event_type, hm_event_type);
    if (ret != 0) {
        TARO_LOG_ERROR("Event", "Invalid event_type:%{public}d ret=%{public}d", event_type, ret);
        return -1;
    }

    auto node = node_owner_.lock();

    ArkUI_NodeHandle ev_node_handle = (param && param->node_handle_) ? param->node_handle_ : node->GetNodeHandle();
    if (ev_node_handle == nullptr) {
        return -1;
    }

    auto node_listener = getNodeListener(ev_node_handle);
    if (node_listener == nullptr) {
        node_listener = std::make_shared<NodeListener>(ev_node_handle);
        node_listeners_[ev_node_handle] = node_listener;
    }

    // store event target
    auto& event_queue = node_listener->event_listeners_[hm_event_type];
    for (const auto& target : event_queue) {
        // 已经存在，直接返回
        if (target.event_type_ == event_type && target.js_event_type_ == js_event_type) {
            return 0;
        }
    }

    TaroEventTarget event_target;
    event_target.event_type_ = event_type;
    event_target.js_event_type_ = js_event_type;
    event_target.with_native_bubble_ = param->with_native_bubble_;
    event_queue.emplace_back(std::move(event_target));

    // 注册鸿蒙事件,只注册一次，不重复注册
    if (event_queue.size() <= 1) {
        auto ret_status = TaroEventAdapter::instance()->registerNodeEvent(
            node->nid_, ev_node_handle, hm_event_type, node->nid_, param ? param->ext_data_ : nullptr);
        TARO_LOG_DEBUG("TaroEvent", "registerEvent js_event_type=%{public}s event=%{public}d node=%{public}p",
                       js_event_type.c_str(), hm_event_type, ev_node_handle);
    }
    return ret;
}

int EventGenerator_HM::unRegisterEvent(const std::string& js_event_type, int event_type, const ArkUI_NodeHandle& node_handle) {
    auto node = node_owner_.lock();
    ArkUI_NodeHandle ev_node_handle = node_handle ? node_handle : node->GetNodeHandle();

    ArkUI_NodeEventType hm_event_type;
    int ret = TaroEventFactory::instance()->getHmEventType(event_type, hm_event_type);
    if (ret != 0) {
        TARO_LOG_ERROR("Event", "Invalid event_type:%{public}d ret=%{public}d", event_type, ret);
        return -1;
    }

    auto node_listener = getNodeListener(ev_node_handle);
    if (node_listener == nullptr) {
        TARO_LOG_DEBUG("TaroEvent", "node_handle is not found");
        return 0;
    }

    auto hm_iter = node_listener->event_listeners_.find(hm_event_type);
    if (hm_iter == node_listener->event_listeners_.end()) {
        TARO_LOG_DEBUG("TaroEvent", "hm_type: %{public}d is not found", hm_event_type);
        return 0;
    }

    auto& targets = hm_iter->second;
    for (auto iter = targets.begin(); iter != targets.end();) {
        if (iter->js_event_type_ == js_event_type && iter->event_type_ == event_type) {
            iter = targets.erase(iter);
        } else {
            iter++;
        }
    }
    if (targets.empty()) {
        TARO_LOG_DEBUG("TaroEvent",
                       "unRegisterEvent js_event_name=%{public}s event=%{public}d node=%{public}p",
                       js_event_type.c_str(), event_type, ev_node_handle);
        TaroEventAdapter::instance()->unRegisterNodeEvent(node->nid_, ev_node_handle, hm_event_type);
        node_listener->event_listeners_.erase(hm_iter);
    }
    return 0;
}

void EventGenerator_HM::dispatchEvent(ArkUI_NodeEvent* event) {
    auto emitter = node_owner_.lock()->getEventEmitter();
    if (emitter == nullptr) {
        return;
    }
    auto node_handle = OH_ArkUI_NodeEvent_GetNodeHandle(event);
    auto node_listener = getNodeListener(node_handle);
    if (node_listener == nullptr) {
        TARO_LOG_ERROR("TaroEvent", "node_handle:%{public}p is not found", node_handle);
        return;
    }

    ArkUI_NodeEventType hw_event_type = OH_ArkUI_NodeEvent_GetEventType(event);
    const auto iter = node_listener->event_listeners_.find(hw_event_type);
    if (iter == node_listener->event_listeners_.end()) {
        TARO_LOG_ERROR("TaroEvent", "hw_event_type:%{public}d is not found", hw_event_type);
        return;
    }

    // 多一次copy，防止回调过程发生更新
    auto vec_events = iter->second;
    auto runner = Render::GetInstance()->GetTaskRunner();
    for (const auto& elem_event : vec_events) {
        std::shared_ptr<TaroEventBase> event_obj = CREATE_TARO_EVENT_OBJ(elem_event.event_type_,
                                                                         elem_event.js_event_type_, event);
        if (event_obj == nullptr) {
            continue;
        }
        event_obj->parseHmEvent(event);
        bool  with_native_bubble =elem_event.with_native_bubble_; 
        if (with_native_bubble) {
            event_obj->revertEventBubble();
        }
        event_obj->target_ = std::dynamic_pointer_cast<TaroElement>(node_owner_.lock());
        if (!event_obj->realTrigger()) {
            continue;
        }
        event_obj->gen_type_ = EventGeneratorType::Event;
        event_obj->event_type = elem_event.event_type_;
        event_obj->js_event_type_ = elem_event.js_event_type_;
        event_obj->node_handle_ = node_handle;
        event_obj->time_mark_ = TaroHelper::TaroTime::getCurrentUsTime();
        // 事件分发
        //         emitter->triggerEvents(event_obj);
        //         TaroEventTask::instance()->pushHmEvent(event_obj);
        //         TaroEventTaskThread::instance()->pushHmEvent(event_obj);
        std::weak_ptr<TaroEventEmitter> weak_emitter = emitter;
        runner->runTask(TaroThread::TaskThread::MAIN, [weak_emitter, event_obj, with_native_bubble]() {
            auto emitter = weak_emitter.lock();
            if (emitter) {
                emitter->triggerEvents(event_obj, with_native_bubble);
            }
        });
    }
    return;
}

EventGenerator_HM::NodeListenerPtr EventGenerator_HM::getNodeListener(ArkUI_NodeHandle node_handle) {
    auto iter = node_listeners_.find(node_handle);
    if (iter == node_listeners_.end()) {
        return nullptr;
    }
    return iter->second;
}
} // namespace TaroRuntime::TaroDOM::TaroEvent