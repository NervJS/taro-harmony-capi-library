/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event.h"

#include "arkjs/ArkJS.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroElementEvent::TaroElementEvent(TaroElement* taroElementRef) {
        event_emitter_ = std::make_shared<TaroEvent::TaroEventEmitter>(taroElementRef);
    }

    TaroElementEvent::~TaroElementEvent() {
        event_emitter_ = nullptr;
        ArkJS arkJs(NativeNodeApi::env);
        for (auto& listener : listeners_ref_) {
            while (listener.second.size() > 0) {
                arkJs.deleteReference(listener.second.back());
                listener.second.pop_back();
            }
        }
        listeners_ref_.clear();
    }

    TaroEvent::TaroEventEmitterPtr TaroElementEvent::getEventEmitter() {
        return event_emitter_;
    }

    int TaroElementEvent::addNodeEventListener(napi_value& js_event_name, napi_value& js_event_handler) {
        ArkJS arkJs(NativeNodeApi::env);
        std::string eventName = arkJs.getString(js_event_name);
        return addNodeEventListener(eventName, js_event_handler);
    }

    int TaroElementEvent::addNodeEventListener(std::string& eventName, napi_value& js_event_handler) {
        ArkJS arkJs(NativeNodeApi::env);
        auto& listeners = listeners_ref_[eventName];
        if (js_event_handler && arkJs.getType(js_event_handler) == napi_function) {
            if (std::find_if(listeners.begin(), listeners.end(), [&](napi_ref ref) {
                    return arkJs.isEqual(arkJs.getReferenceValue(ref), js_event_handler);
                }) == listeners.end()) {
                if (listeners.empty()) {
                    bool is_bind = bindListenEvent(eventName);
                    if (!is_bind) {
                        getEventEmitter()->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, eventName);
                    }
                }
                listeners.push_back(arkJs.createReference(js_event_handler));
            }
        }
        return 0;
    }

    int TaroElementEvent::removeNodeEventListener(napi_value& js_event_name, napi_value& js_event_handler) {
        ArkJS arkJs(NativeNodeApi::env);
        std::string eventName = arkJs.getString(js_event_name);
        return removeNodeEventListener(eventName, js_event_handler);
    }

    int TaroElementEvent::removeNodeEventListener(std::string& eventName, napi_value& js_event_handler) {
        ArkJS arkJs(NativeNodeApi::env);
        auto& listeners = listeners_ref_[eventName];
        if (js_event_handler && arkJs.getType(js_event_handler) == napi_function) {
            auto it = std::remove_if(listeners.begin(), listeners.end(), [&](napi_ref ref) {
                if (arkJs.isEqual(arkJs.getReferenceValue(ref), js_event_handler)) {
                    arkJs.deleteReference(ref);
                    return true;
                }
                return false;
            });
            listeners.erase(it, listeners.end());
        } else {
            for (auto listener : listeners) {
                arkJs.deleteReference(listener);
            }
            listeners.clear();
        }
        if (listeners.empty()) {
            return getEventEmitter()->unRegisterEventByName(eventName, nullptr);
        }
        return 0;
    }

    int TaroElementEvent::updateNodeEventListener(std::string& eventName, napi_value& oddListener, napi_value& newListener) {
        ArkJS arkJs(NativeNodeApi::env);
        auto& listeners = listeners_ref_[eventName];
        if (!arkJs.isEqual(oddListener, newListener)) {
            auto it = std::remove_if(listeners.begin(), listeners.end(), [&](napi_ref ref) {
                if (arkJs.isEqual(arkJs.getReferenceValue(ref), oddListener)) {
                    arkJs.deleteReference(ref);
                    return true;
                }
                return false;
            });
            listeners.erase(it, listeners.end());
            listeners.push_back(arkJs.createReference(newListener));
        }
        return 0;
    }

    void TaroElementEvent::dispatchNodeEvent(TaroEvent::TaroEventPtr event) {
        ArkJS ark_js(NativeNodeApi::env);
        const auto iter = listeners_ref_.find(event->js_event_type_);
        if (iter == listeners_ref_.end()) {
            return;
        }
        const auto listeners = iter->second;
        for (const auto& listener : listeners) {
            auto callback = ark_js.getReferenceValue(listener);
            TaroElementRef el = event->cur_target_;
            ark_js.call(callback, {event->GetNapiValue()}, el->GetNodeValue());
            event->jsToMem_Partition();
            if (event->end_) {
                break;
            }
        }
    }

    // int TaroElementEvent::updateNodeEvents() {
    //     if (!has_reg) {
    //         for (const auto& listener : listeners_ref_) {
    //             bool bind_succ = bindListenEvent(listener.first);
    //             if (!bind_succ) {
    //                 getEventEmitter()->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, listener.first);
    //             }
    //         }
    //     }
    //     has_reg = true;
    //     return 0;
    // }

} // namespace TaroDOM
} // namespace TaroRuntime
