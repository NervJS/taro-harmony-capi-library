//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef HARMONY_LIBRARY_EVENT_RADIO_H
#define HARMONY_LIBRARY_EVENT_RADIO_H
#include "runtime/dom/event/event_hm/event_base.h"
namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroRadioEventChange : public TaroEventBase {
            public:
            TaroRadioEventChange(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroEventBase(
                      js_event_type, event) {
                bubbles_ = false;
            }
            virtual ~TaroRadioEventChange() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            private:
            std::optional<std::string> value_;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_EVENT_RADIO_H
