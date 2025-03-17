//
// Created by wangzhongqiang on 24-4-28.
//
#pragma once

#include <functional>
#include <map>
#include <string>
#include <arkui/native_node.h>
#include <napi/native_api.h>

#include "../taro_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        // 事件回调数据存储类
        class TaroEventBase : public TaroEvent {
            public:
            TaroEventBase(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventBase() {}

            public:
            virtual bool realTrigger();
    
            virtual int parseHmEvent(ArkUI_NodeEvent *event);

            // virtual int parseFun();
            virtual int serializeFun(napi_value &ret_val);

            int serializeFun() override;
    
            ArkUI_NodeEvent * getOriginEvent () {
                return event_;
            }
    
            virtual void revertEventBubble () {}

            napi_value &detail();

            public:
            ArkUI_NodeEventType hm_event_type_;
            uint64_t time_mark_ = 0;

            private:
            ArkUI_NodeEvent *event_ = nullptr;

            private:
            napi_value js_detail_ = nullptr;
        };
        using TaroEventBasePtr = std::shared_ptr<TaroEventBase>;
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
