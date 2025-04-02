/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventTouch : public TaroEventBase {
            public:
            TaroEventTouch(const std::string& js_event_type, ArkUI_NodeEvent* event);

            virtual ~TaroEventTouch() = default;

            int parseHmEvent(ArkUI_NodeEvent* event) override;

            int serializeFun(napi_value& ret_obj) override;

            void revertEventBubble() override;

            bool realTrigger() override;

            public:
            int hm_touch_type_ = UI_TOUCH_EVENT_ACTION_UP;
            int event_touch_type = -1;
            float client_x_ = 0.0f;
            float client_y_ = 0.0f;
            float page_x_ = 0.0f;
            float page_y_ = 0.0f;
            float screen_x_ = 0.0f;
            float screen_y_ = 0.0f;
        };

        // 各touch类型子事件
        class TaroEventTouchStart : public TaroEventTouch {
            public:
            TaroEventTouchStart(const std::string& event_type, ArkUI_NodeEvent* event)
                : TaroEventTouch(event_type,
                                 event) {
                hm_touch_type_ = UI_TOUCH_EVENT_ACTION_DOWN;
            }
        };

        class TaroEventTouchEnd : public TaroEventTouch {
            public:
            TaroEventTouchEnd(const std::string& event_type, ArkUI_NodeEvent* event)
                : TaroEventTouch(event_type,
                                 event) {
                hm_touch_type_ = UI_TOUCH_EVENT_ACTION_UP;
            }
        };

        class TaroEventTouchMove : public TaroEventTouch {
            public:
            TaroEventTouchMove(const std::string& event_type, ArkUI_NodeEvent* event)
                : TaroEventTouch(event_type,
                                 event) {
                hm_touch_type_ = UI_TOUCH_EVENT_ACTION_MOVE;
            }
        };

        class TaroEventTouchCancel : public TaroEventTouch {
            public:
            TaroEventTouchCancel(const std::string& event_type, ArkUI_NodeEvent* event)
                : TaroEventTouch(event_type,
                                 event) {
                hm_touch_type_ = UI_TOUCH_EVENT_ACTION_CANCEL;
            }
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
