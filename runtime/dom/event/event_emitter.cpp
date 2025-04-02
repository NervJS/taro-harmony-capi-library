/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_emitter.h"

#include "arkjs/ArkJS.h"
#include "event_hm/event_generator_hm.h"
#include "event_hm/event_types/event_areachange.h"
#include "event_hm/event_types/event_type.h"
#include "event_user/event_generator_user.h"
#include "gesture/event_generator_gesture.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroEventEmitter::TaroEventEmitter(TaroElement* node)
    : node_owner_(node) {}

TaroEventEmitter::~TaroEventEmitter() {
}

int TaroEventEmitter::addEventListener(const std::string& js_event_type, EventListenParams* params, bool call_js) {
    EventGeneratorType gen_type = EventGeneratorType::UserDefine;
    auto node_handle = (!params || !params->node_handle_) ? node_owner_->GetNodeHandle() : params->node_handle_;
    if (node_handle == nullptr) {
        TARO_LOG_DEBUG("TaroEvent", "node_handle is null:%{public}d %{public}s", node_owner_->nid_, node_owner_->GetNodeName().c_str());
        return -1;
    }
    int event_type = -1;
    if (params != nullptr) {
        gen_type = params->gen_type_;
        event_type = params->event_type_;
    }

    // 到generator注册
    EventGeneratorPtr generator = getGenerator(gen_type);
    if (generator == nullptr) {
        generator = createGenerator(gen_type);
        generators_[gen_type] = generator;
    }

    int ret = generator->registerEvent(js_event_type, event_type, params);
    if (ret != 0) {
        TARO_LOG_ERROR("TaroEvent", "register failed, js_type:%{public}s type:%{public}d", js_event_type.c_str(), event_type);
        return ret;
    }

    // 添加listener
    EventListenersPtr listeners = nullptr;
    auto listen_iter = listeners_.find(js_event_type);
    if (listen_iter == listeners_.end()) {
        listeners = std::make_shared<EventListeners>();
        listeners_[js_event_type] = listeners;
    } else {
        listeners = listen_iter->second;
    }
    EventListener listener;
    listener.call_js_ = call_js;
    listener.gen_type_ = gen_type;
    listener.event_type_ = event_type;
    listener.node_handle_ = node_handle;
    listener.node_event_fun_ = params ? params->event_fun_ : nullptr;

    listeners->listeners_.emplace_back(std::move(listener));

    return 0;
}

void TaroEventEmitter::removeEventListener(const std::string& js_event_type, bool call_js, const ArkUI_NodeHandle& node_handle) {
    auto iter_listeners = listeners_.find(js_event_type);
    if (iter_listeners == listeners_.end() || iter_listeners->second == nullptr) {
        return;
    }
    auto& listeners = iter_listeners->second->listeners_;
    for (auto iter = listeners.begin(); iter != listeners.end();) {
        if (iter->call_js_ != call_js || (node_handle != nullptr && iter->node_handle_ != node_handle)) {
            iter++;
            continue;
        }
        auto event_type = iter->event_type_;
        auto gen_type = iter->gen_type_;
        auto ev_node_handle = iter->node_handle_;
        iter = listeners.erase(iter);
        // 查找相同类型事件是否还存在
        auto find_iter = std::find_if(listeners.begin(), listeners.end(), [event_type, gen_type, ev_node_handle](const EventListener& listener) {
            return listener.gen_type_ == gen_type && listener.event_type_ == event_type && listener.node_handle_ == ev_node_handle;
        });
        if (find_iter != listeners.end()) {
            continue;
        }
        // 不存在，做删除
        auto generator = getGenerator(gen_type);
        if (generator) {
            generator->unRegisterEvent(js_event_type, event_type, ev_node_handle);
        }
    }
}

int TaroEventEmitter::triggerEvents(TaroEventPtr event, bool with_native_bubble) {
    if (event == nullptr) {
        return -1;
    }

    // TARO_LOG_DEBUG("TaroEvent", "wzq event: %{public}s %{public}d", event->js_event_type_.c_str(), event->gen_type_);

    event->target_ = std::dynamic_pointer_cast<TaroElement>(node_owner_->shared_from_this());
    event->cur_target_ = event->target_;
    dispatchEvents_First(event);

    if (with_native_bubble)
        return 0;

    auto node = std::dynamic_pointer_cast<TaroElement>(node_owner_->GetParentNode());
    while (node) {
        if (!event->bubbles_ || event->stop_) {
            break;
        }
        // 分发事件
        event->cur_target_ = node;
        node->getEventEmitter()->dispatchEvents(event);
        node = std::dynamic_pointer_cast<TaroElement>(node->GetParentNode());
    }
    return 0;
}

// 分发no callback事件
bool TaroEventEmitter::dispatchEvents_First(TaroEventPtr event) {
    auto listeners_iter = listeners_.find(event->js_event_type_);
    if (listeners_iter == listeners_.end()) {
        return false;
    }

    // todo: disabled 需直接返回

    bool has_call_js = false;
    // 此处做赋值，防止回调过程中listeners_发生改变
    auto listeners_ptr = listeners_iter->second;
    auto listeners = listeners_ptr->listeners_;
    for (auto& listener : listeners) {
        // 不是定义的触发
        if (listener.gen_type_ != event->gen_type_ || listener.event_type_ != event->event_type || (event->node_handle_ && listener.node_handle_ != event->node_handle_)) {
            continue;
        }

        if (event->stop_ && event->end_ && listener.call_js_) {
            continue;
        }

        // 判断监听释放还有效
        if (!isValid(listeners_ptr, listener)) {
            continue;
        }

        if (listener.node_event_fun_) {
            int ret = listener.node_event_fun_(event);
            if (ret != 0) {
                event->stop_ = true;
                continue;
            }
        }

        if (!listener.call_js_) {
            continue;
        }

        has_call_js = true;
        event->recallJS();
    }
    return has_call_js;
}

int TaroEventEmitter::dispatchEvents(TaroEventPtr event) {
    if (event == nullptr) {
        return -1;
    }

    event->cur_target_ = std::dynamic_pointer_cast<TaroElement>(node_owner_->shared_from_this());

    auto js_event_type = event->js_event_type_;
    auto iter = listeners_.find(js_event_type);
    if (iter == listeners_.end() || iter->second == nullptr) {
        // TARO_LOG_DEBUG("EventEmitter", "event:%{public}s has not listener", js_event_type.c_str());
        return 0;
    }

    // 此处做赋值，防止回调过程中listeners_发生改变
    auto listeners_ptr = iter->second;
    auto listeners = listeners_ptr->listeners_;

    // 触发监听
    for (auto& elem : listeners) {
        if (!elem.call_js_) {
            continue;
        }

        // 判断监听释放还有效
        if (!isValid(listeners_ptr, elem)) {
            continue;
        }

        if (elem.node_event_fun_ != nullptr) {
            elem.node_event_fun_(event);
        }
        // 回调JS
        event->recallJS();
    }
    return 0;
}

int TaroEventEmitter::bindEvent(TaroElementRef element, const std::string& js_event_type) {
    bool bret = element->bindListenEvent(js_event_type);
    if (bret) {
        return 0;
    }
    EventGeneratorType gen_type = EventGeneratorType::UserDefine;
    int event_type = -1;
    getType(js_event_type, gen_type, event_type);
    // 注册事件
    EventListenParams params;
    params.event_type_ = event_type;
    params.gen_type_ = gen_type;
    addEventListener(js_event_type, &params, true);
    return 0;
}

bool TaroEventEmitter::getType(const std::string& js_event_type, EventGeneratorType& gen_type, int& event_type) {
    static std::unordered_map<std::string, std::pair<EventGeneratorType, int> > s_map_gen = {
        {"click", {EventGeneratorType::Event, TARO_EVENT_TYPE_CLICK}}};
    auto iter = s_map_gen.find(js_event_type);
    if (iter != s_map_gen.end()) {
        gen_type = iter->second.first;
        event_type = iter->second.second;
    } else {
        gen_type = EventGeneratorType::UserDefine;
        event_type = -1;
    }
    return true;
}

EventGeneratorPtr TaroEventEmitter::createGenerator(EventGeneratorType gen_type) {
    switch (gen_type) {
        case EventGeneratorType::Event: {
            return std::make_shared<EventGenerator_HM>(
                std::dynamic_pointer_cast<TaroElement>(node_owner_->shared_from_this()));
        } break;
        case EventGeneratorType::Gesture: {
            return std::make_shared<EventGenerator_Gesture>(
                std::dynamic_pointer_cast<TaroElement>(node_owner_->shared_from_this()));
        } break;
        case EventGeneratorType::UserDefine: {
            return std::make_shared<EventGenerator_User>(
                std::dynamic_pointer_cast<TaroElement>(node_owner_->shared_from_this()));
        } break;
        default:
            return nullptr;
    }
    return nullptr;
}

EventGeneratorPtr TaroEventEmitter::getGenerator(EventGeneratorType gen_type) {
    auto iter = generators_.find(gen_type);
    if (iter != generators_.end()) {
        return iter->second;
    }
    return nullptr;
}

int TaroEventEmitter::registerEvent(int event_type, const std::string& js_event_type,
                                    const HMEventCallFun& node_event_fun, const ArkUI_NodeHandle& node_handle) {
    EventListenParams params;
    params.event_type_ = event_type;
    if (node_event_fun) {
        params.event_fun_ = [node_event_fun](TaroEventPtr event_base) {
            auto event = dynamic_pointer_cast<TaroEventBase>(event_base);
            if (event == nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event type");
                return -1;
            }
            auto event_obj = event->GetNapiValue();
            return node_event_fun(event, event_obj);
        };
    }
    params.node_handle_ = node_handle;
    params.gen_type_ = EventGeneratorType::Event;
    return addEventListener(js_event_type, &params, true);
}

int TaroEventEmitter::registerEvent_NoCallBack(int event_type, const std::string& js_event_type,
                                               const HMEventCallFun& node_event_fun, const ArkUI_NodeHandle& node_handle) {
    EventListenParams params;
    params.event_type_ = event_type;
    if (node_event_fun) {
        params.event_fun_ = [node_event_fun](TaroEventPtr event_base) {
            auto event = dynamic_pointer_cast<TaroEventBase>(event_base);
            if (event == nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event type");
                return -1;
            }
            auto event_obj = event->GetNapiValue();
            return node_event_fun(event, event_obj);
        };
    }
    params.node_handle_ = node_handle;
    params.gen_type_ = EventGeneratorType::Event;
    return addEventListener(js_event_type, &params, false);
}

int TaroEventEmitter::registerEvent_NoCallBackWithBubble(int event_type, const std::string& js_event_type, const HMEventCallFun& node_event_fun) {
    EventListenParams params;
    params.event_type_ = event_type;
    if (node_event_fun) {
        params.event_fun_ = [node_event_fun](TaroEventPtr event_base) {
            auto event = dynamic_pointer_cast<TaroEventBase>(event_base);
            if (event == nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event type");
                return -1;
            }
            auto event_obj = event->GetNapiValue();
            return node_event_fun(event, event_obj);
        };
    }
    params.with_native_bubble_ = true;
    params.gen_type_ = EventGeneratorType::Event;
    return addEventListener(js_event_type, &params, false);
}

int TaroEventEmitter::registerEvent_NoCallBack(int event_type, const std::string& js_event_type, void* args,
                                               const HMEventCallFun& node_event_fun, const ArkUI_NodeHandle& node_handle) {
    EventListenParams params;
    params.event_type_ = event_type;
    if (node_event_fun) {
        params.event_fun_ = [node_event_fun](TaroEventPtr event_base) {
            auto event = dynamic_pointer_cast<TaroEventBase>(event_base);
            if (event == nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event type");
                return -1;
            }
            auto event_obj = event->GetNapiValue();
            return node_event_fun(event, event_obj);
        };
    }
    params.node_handle_ = node_handle;
    params.ext_data_ = args;
    params.gen_type_ = EventGeneratorType::Event;
    return addEventListener(js_event_type, &params, false);
}

int TaroEventEmitter::unRegisterEventByName(const std::string& js_event_name, const ArkUI_NodeHandle& node_handle) {
    removeEventListener(js_event_name, true, node_handle);
    return 0;
}

int TaroEventEmitter::unRegisterEventByName_NoCallBack(const std::string& js_event_name,
                                                       const ArkUI_NodeHandle& node_handle) {
    removeEventListener(js_event_name, false, node_handle);
    return 0;
}

int TaroEventEmitter::registerEvent_Once(const std::string& js_event_type, napi_ref& callback_ref) {
    static std::unordered_map<std::string, int> event_map = {
        {"area_change", TARO_EVENT_TYPE_AREA_CHANGE},
        {"visible_area_change", TARO_EVENT_TYPE_VISIBLE_AREA_CHANGE}};
    auto it = event_map.find(js_event_type);
    if (it != event_map.end()) {
        auto func = [this, &js_event_type, env = NativeNodeApi::env, callback_ref](std::shared_ptr<TaroEvent> event) mutable -> int {
            napi_value js_event;
            napi_create_object(env, &js_event);
            std::string event_type = event->js_event_type_;
            if (event_type == "area_change") {
                auto event_obj = std::dynamic_pointer_cast<TaroEventAreaChange>(event);
                event_obj->serializeFun(js_event);
            } else {
                napi_create_object(env, &js_event);
            }

            napi_value args[1];
            args[0] = js_event;
            napi_value result;
            napi_value callback;
            napi_get_reference_value(env, callback_ref, &callback);
            napi_call_function(env, nullptr, callback, 1, args, &result);
            if (callback_ref != nullptr) {
                ArkJS arkJs(env);
                arkJs.deleteReference(callback_ref);
                callback_ref = nullptr;
            }
            //  删除事件
            unRegisterEventByName_NoCallBack(event_type, node_owner_->GetNodeHandle());
            return 0;
        };
        EventListenParams params;
        params.event_type_ = it->second;
        params.gen_type_ = EventGeneratorType::Event;
        params.node_handle_ = node_owner_->GetNodeHandle();
        params.event_fun_ = func;
        addEventListener(js_event_type, &params, false);
        //         registerEvent_NoCallBack(it->second, js_event_type, func, node_owner_.lock()->GetNodeHandle());
    } else {
        TARO_LOG_ERROR("Event", "Unknown event type: " << js_event_type);
        return -1;
    }

    return 0;
}

void TaroEventEmitter::clearNodeEvent(const ArkUI_NodeHandle& node_handle) {
    for (auto iter = listeners_.begin(); iter != listeners_.end();) {
        auto& listeners = iter->second->listeners_;
        for (auto listener_iter = listeners.begin(); listener_iter != listeners.end();) {
            if (listener_iter->node_handle_ == node_handle) {
                auto generator = getGenerator(listener_iter->gen_type_);
                generator->unRegisterEvent(iter->first, listener_iter->event_type_, node_handle);
                listener_iter = listeners.erase(listener_iter);
            } else {
                listener_iter++;
            }
        }
        if (listeners.empty()) {
            iter = listeners_.erase(iter);
        } else {
            iter++;
        }
    }
}

int TaroEventEmitter::registerEvent(EventGeneratorType gen_type, int event_type,
                                    const std::string& js_event_type, const EventCallFun& node_event_fun,
                                    const ArkUI_NodeHandle& node_handle, void* args) {
    EventListenParams params;
    params.event_type_ = event_type;
    params.event_fun_ = node_event_fun;
    params.node_handle_ = node_handle;
    params.gen_type_ = gen_type;
    params.ext_data_ = args;
    return addEventListener(js_event_type, &params, true);
}

int TaroEventEmitter::registerEvent_NoCallBack(EventGeneratorType gen_type, int event_type, const std::string& js_event_type,
                                               const EventCallFun& node_event_fun, const ArkUI_NodeHandle& node_handle, void* args) {
    EventListenParams params;
    params.gen_type_ = gen_type;
    params.event_type_ = event_type;
    params.event_fun_ = node_event_fun;
    params.node_handle_ = node_handle;
    params.ext_data_ = args;

    return addEventListener(js_event_type, &params, false);
}

bool TaroEventEmitter::isValid(EventListenersPtr listener, const EventListener& listen) {
    if (!listener) {
        return false;
    }
    for (const auto& elem : listener->listeners_) {
        if (elem == listen) {
            return true;
        }
    }
    return false;
}

int TaroEventEmitter::triggerEvent(napi_env env, napi_value n_val) {
    napi_valuetype value_type = napi_undefined;
    napi_typeof(env, n_val, &value_type);
    if (value_type != napi_object) {
        TARO_LOG_ERROR("TaroEvent", "type:%{public}d is not object", value_type);
        return -1;
    }
    auto generator = getGenerator(EventGeneratorType::UserDefine);
    if (!generator) {
        generator = createGenerator(EventGeneratorType::UserDefine);
        generators_[EventGeneratorType::UserDefine] = generator;
    }
    auto generator_user = std::static_pointer_cast<EventGenerator_User>(generator);
    generator_user->triggerEvent(n_val);
    return 0;
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
