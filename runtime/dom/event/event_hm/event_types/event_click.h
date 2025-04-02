/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventClick : public TaroEventBase {
            public:
            TaroEventClick(const std::string& js_event_type, ArkUI_NodeEvent* event)
                : TaroEventBase(js_event_type,
                                event) {}

            virtual ~TaroEventClick() {}

            int parseHmEvent(ArkUI_NodeEvent* event) override;

            int serializeFun(napi_value& ret_obj) override;

            public:
            float x_ = 0.0f;
            float y_ = 0.0f;
            float client_x_ = 0.0f;
            float client_y_ = 0.0f;
            float page_x_ = 0.0f;
            float page_y_ = 0.0f;
            float screen_x_ = 0.0f;
            float screen_y_ = 0.0f;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
