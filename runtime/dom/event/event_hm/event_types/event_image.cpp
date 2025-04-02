/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_image.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventCompleteOnImage::TaroEventCompleteOnImage(const std::string& js_event_type, ArkUI_NodeEvent* event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventCompleteOnImage::parseHmEvent(ArkUI_NodeEvent* event) {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get component_event failed");
                return -1;
            }

            loading_status_ = component_event->data[0].i32;
            width_ = component_event->data[1].f32;
            height_ = component_event->data[2].f32;
            component_width_ = component_event->data[3].f32;
            component_height_ = component_event->data[4].f32;
            content_offset_x_ = component_event->data[5].f32;
            content_offset_y_ = component_event->data[6].f32;
            content_width_ = component_event->data[7].f32;
            content_height_ = component_event->data[8].f32;
            return 0;
        }

        int TaroEventCompleteOnImage::serializeFun(napi_value& ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            NapiSetter::SetProperty(ret_obj, "loadingStatus", loading_status_);
            NapiSetter::SetProperty(ret_obj, "width", width_);
            NapiSetter::SetProperty(ret_obj, "height", height_);
            NapiSetter::SetProperty(ret_obj, "componentWidth", component_width_);
            NapiSetter::SetProperty(ret_obj, "componentHeight", component_height_);
            NapiSetter::SetProperty(ret_obj, "contentOffsetX", content_offset_x_);
            NapiSetter::SetProperty(ret_obj, "contentOffsetY", content_offset_y_);
            NapiSetter::SetProperty(ret_obj, "contentWidth", content_width_);
            NapiSetter::SetProperty(ret_obj, "contentHeight", content_height_);
            return 0;
        }

    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
