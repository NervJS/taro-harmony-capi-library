/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <functional>
#include <string>

#include "runtime/dom/event/event_emitter.h"
#include "runtime/dom/event/taro_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroElementEvent {
        public:
        TaroElementEvent(TaroElement *taroElementRef);

        virtual ~TaroElementEvent();

        std::unordered_map<std::string, std::vector<napi_ref>> listeners_ref_;

        // 获取事件分发
        TaroEvent::TaroEventEmitterPtr getEventEmitter();

        virtual bool bindListenEvent(const std::string &event_name) {
            return false;
        }

        // int updateNodeEvents();

        int addNodeEventListener(napi_value &js_event_name, napi_value &js_event_handler);
        int addNodeEventListener(std::string &eventName, napi_value &js_event_handler);
        int removeNodeEventListener(napi_value &js_event_type, napi_value &js_event_handler);
        int removeNodeEventListener(std::string &eventName, napi_value &js_event_handler);
        int updateNodeEventListener(std::string &eventName, napi_value &oddListener, napi_value &newListener);

        virtual void dispatchNodeEvent(TaroEvent::TaroEventPtr event);

        TaroEvent::TaroEventEmitterPtr event_emitter_ = nullptr;
        //         bool has_reg = false;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
