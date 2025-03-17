//
// Created on 2024/6/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_picker.h"

#include "helper/TaroLog.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroPickerChangeEvent::TaroPickerChangeEvent(
            const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
            bubbles_ = false;
        }

        int TaroPickerChangeEvent::parseHmEvent(ArkUI_NodeEvent *event) {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                return -1;
            }
            currentSelector = component_event->data[0].i32;
            comp_event_ = *component_event;
            return 0;
        }

        int TaroPickerChangeEvent::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            auto &js_detail = detail();
            NapiSetter::SetProperty(js_detail, "value", currentSelector);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
