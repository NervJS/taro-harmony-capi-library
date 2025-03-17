//
// Created by wangzhongqiang on 2024/5/10.
//

#pragma once

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventCompleteOnImage : public TaroEventBase {
            public:
            TaroEventCompleteOnImage(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventCompleteOnImage() {}
    
            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            public:
            int32_t loading_status_ = 0.0f;
            float width_ = 0.0f;
            float height_ = 0.0f;
            float component_width_ = 0.0f;
            float component_height_ = 0.0f;
            float content_offset_x_ = 0.0f;
            float content_offset_y_ = 0.0f;
            float content_width_ = 0.0f;
            float content_height_ = 0.0f;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
