//
// Created by wangzhongqiang on 24-4-28.
//
#pragma once

#include <functional>
#include <map>
#include <string>
#include <arkui/native_node.h>

#include "event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventFactory final {
            public:
            using EventCreateFun = std::function<TaroEventBase *(const std::string &js_event_type,
                                                                 ArkUI_NodeEvent *event)>;

            public:
            static TaroEventFactory *instance();

            TaroEventFactory();

            ~TaroEventFactory() {}

            int registerHandler(int event_type, ArkUI_NodeEventType hm_ev_type, const EventCreateFun &new_fun);

            std::shared_ptr<TaroEventBase> createEventDataObj(int event_type, const std::string &js_event_type,
                                                              ArkUI_NodeEvent *event);

            int getHmEventType(const int, ArkUI_NodeEventType &hm_event_type);

            struct TaroEventHelper {
                int event_type_;
                ArkUI_NodeEventType hm_event_type_;
                EventCreateFun event_create_fun_ = nullptr;
            };

            private:
            std::vector<TaroEventHelper *> parser_factory_;
        };

        class TaroEventRegisterHelper {
            public:
            TaroEventRegisterHelper(int event_type, ArkUI_NodeEventType hm_event_type,
                                    const TaroEventFactory::EventCreateFun &create_fun) {
                TaroEventFactory::instance()->registerHandler(event_type, hm_event_type, create_fun);
            }
        };

// 注册各事件解析函数、序列函数、数据类
#define REGISTER_TAROEVENT(event_type, hm_event_type, TaroEventClass)                                                                                 \
    static TaroEventRegisterHelper TaroEventRegisterHelper##event_type(event_type, hm_event_type,                                                     \
                                                                       [](const std::string &js_ev_type, ArkUI_NodeEvent *event) -> TaroEventBase * { \
                                                                           return (TaroEventBase *)(new TaroEventClass(js_ev_type, event));           \
                                                                       });
#define CREATE_TARO_EVENT_OBJ(event_type, js_event_type, hm_event) \
    TaroEventFactory::instance()->createEventDataObj(event_type, js_event_type, hm_event);

    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
