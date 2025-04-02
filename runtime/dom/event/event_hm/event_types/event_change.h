/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroTextInputEventChange : public TaroEventBase {
            public:
            TaroTextInputEventChange(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroEventBase(js_event_type, event) {
                bubbles_ = false;
            }

            virtual ~TaroTextInputEventChange() {}

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;

            const std::string& getValue();

            private:
            std::string value_;
        };

        class TaroTextInputEventInput : public TaroTextInputEventChange {
            public:
            TaroTextInputEventInput(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroTextInputEventChange(js_event_type, event) {}

            bool realTrigger() override;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
