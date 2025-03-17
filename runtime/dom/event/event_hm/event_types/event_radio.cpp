//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "event_radio.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"
namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        int TaroRadioEventChange::parseHmEvent(ArkUI_NodeEvent *event) {
            auto ptr_ev_string = OH_ArkUI_NodeEvent_GetStringAsyncEvent(event);
            if (ptr_ev_string != nullptr && ptr_ev_string->pStr != nullptr) {
                value_ = ptr_ev_string->pStr;
            }
            return 0;
        }

        int TaroRadioEventChange::serializeFun(napi_value &ret_obj) {
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
