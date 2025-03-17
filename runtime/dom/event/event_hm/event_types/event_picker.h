//
// Created on 2024/6/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroPickerChangeEvent : public TaroEventBase {
            public:
            TaroPickerChangeEvent(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroPickerChangeEvent() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            int parseEvent();

            public:
            int32_t currentSelector;
            ArkUI_NodeComponentEvent comp_event_;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
