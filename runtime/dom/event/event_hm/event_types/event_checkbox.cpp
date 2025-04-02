/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_checkbox.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"
namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        int TaroCheckboxEventChange::parseHmEvent(ArkUI_NodeEvent *event) {
            ArkUI_StringAsyncEvent *ptr_ev_string = OH_ArkUI_NodeEvent_GetStringAsyncEvent(event);
            if (ptr_ev_string != nullptr && ptr_ev_string->pStr != nullptr) {
                value_ = ptr_ev_string->pStr;
            }

            ArkUI_NodeComponentEvent *event_data = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            selected_value_ = event_data->data[0].i32;
            return 0;
        }

        int TaroCheckboxEventChange::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            napi_value js_detail = detail();
            if (value_.has_value()) {
                NapiSetter::SetProperty(js_detail, "value", value_.value());
            }
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
