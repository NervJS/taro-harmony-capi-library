/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
