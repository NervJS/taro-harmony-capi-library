//
// Created on 2024/6/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_EVENT_INPUT_H
#define TARO_CAPI_HARMONY_DEMO_EVENT_INPUT_H

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroInputFocus : public TaroEventBase {
            public:
            TaroInputFocus(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroInputFocus();
    
            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            private:
            std::string value_;
        };

        class TaroInputBlur : public TaroEventBase {
            public:
            TaroInputBlur(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroInputBlur();
    
            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;
            private:
            std::string value_;
        };

        class TaroInputSubmit : public TaroEventBase {
            public:
            TaroInputSubmit(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroInputSubmit();
    
            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;
            private:
            std::string value_;
//             int32_t enterType_;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_EVENT_INPUT_H
