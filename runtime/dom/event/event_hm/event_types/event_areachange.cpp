//
// Created on 2024/4/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_areachange.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventAreaChange::TaroEventAreaChange(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventAreaChange::parseHmEvent(ArkUI_NodeEvent *event) {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                return -1;
            }

            old_width_ = component_event->data[0].f32;
            old_height_ = component_event->data[1].f32;
            old_parent_x_ = component_event->data[2].f32;
            old_parent_y_ = component_event->data[3].f32;
            old_page_x_ = component_event->data[4].f32;
            old_page_y_ = component_event->data[5].f32;
            new_width_ = component_event->data[6].f32;
            new_height_ = component_event->data[7].f32;
            new_parent_x_ = component_event->data[8].f32;
            new_parent_y_ = component_event->data[9].f32;
            new_page_x_ = component_event->data[10].f32;
            new_page_y_ = component_event->data[11].f32;
        }

        int TaroEventAreaChange::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            NapiSetter::SetProperty(ret_obj, "originWidth", old_width_);
            NapiSetter::SetProperty(ret_obj, "originHeight", old_height_);
            NapiSetter::SetProperty(ret_obj, "originXWithParentElement", old_parent_x_);
            NapiSetter::SetProperty(ret_obj, "originYWithParentElement", old_parent_y_);
            NapiSetter::SetProperty(ret_obj, "originXWithParent", old_page_x_);
            NapiSetter::SetProperty(ret_obj, "originYWithParent", old_page_y_);
            NapiSetter::SetProperty(ret_obj, "newWidth", new_width_);
            NapiSetter::SetProperty(ret_obj, "newHeight", new_height_);
            NapiSetter::SetProperty(ret_obj, "newXWithParentElement", new_parent_x_);
            NapiSetter::SetProperty(ret_obj, "newYWithParentElement", new_parent_y_);
            NapiSetter::SetProperty(ret_obj, "newXWithParent", new_page_x_);
            NapiSetter::SetProperty(ret_obj, "newYWithParent", new_page_y_);

            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
