/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_EVENT_SWIPER_H
#define TARO_CAPI_HARMONY_DEMO_EVENT_SWIPER_H

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroElementChangedInSwiper : public TaroEventBase {
            public:
            TaroElementChangedInSwiper(const std::string& js_event_type,
                                       ArkUI_NodeEvent* event);

            virtual ~TaroElementChangedInSwiper();

            int serializeFun(napi_value& ret_obj) override;

            int parseHmEvent(ArkUI_NodeEvent* event) override;

            int32_t current_;
        };

        class TaroEventTransitionInSwiper final : public TaroEventBase {
            public:
            TaroEventTransitionInSwiper(const std::string& js_event_type,
                                        ArkUI_NodeEvent* event);

            virtual ~TaroEventTransitionInSwiper() = default;

            int parseHmEvent(ArkUI_NodeEvent* event) override;

            int serializeFun(napi_value& ret_obj) override;

            float move_val_ = 0.0;
            int current_index_ = 0;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_EVENT_SWIPER_H
