/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_EVENT_SLIDER_H
#define TARO_CAPI_HARMONY_DEMO_EVENT_SLIDER_H

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroSliderEventChange : public TaroEventBase {
            public:
            TaroSliderEventChange(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroEventBase(
                      js_event_type, event) {
                bubbles_ = false;
            }
            virtual ~TaroSliderEventChange() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            const float &getValue();

            virtual bool realTrigger() override;

            protected:
            float value_ = 0.0;
            int state_ = 0.0;
        };

        class TaroSliderEventChanging : public TaroSliderEventChange {
            public:
            TaroSliderEventChanging(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroSliderEventChange(js_event_type, event) {
            }
            virtual ~TaroSliderEventChanging() = default;

            virtual bool realTrigger() override;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_EVENT_SLIDER_H
