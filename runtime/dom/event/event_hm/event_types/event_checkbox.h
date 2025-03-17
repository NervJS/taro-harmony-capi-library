//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef HARMONY_LIBRARY_EVENT_CHECKBOX_H
#define HARMONY_LIBRARY_EVENT_CHECKBOX_H
#include "runtime/dom/event/event_hm/event_base.h"
namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroCheckboxEventChange : public TaroEventBase {
            public:
            TaroCheckboxEventChange(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroEventBase(
                      js_event_type, event) {
                bubbles_ = false;
            }
            virtual ~TaroCheckboxEventChange() {}

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            public:
            std::optional<std::string> value_;
            int32_t selected_value_;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_EVENT_CHECKBOX_H
