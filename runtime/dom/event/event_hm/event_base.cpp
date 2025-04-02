/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_base.h"

#include "event_factory.h"
#include "helper/TaroLog.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventBase::TaroEventBase(const std::string& js_event_type, ArkUI_NodeEvent* event)
            : event_(event) {
            gen_type_ = EventGeneratorType::Event;
            js_event_type_ = js_event_type;
            hm_event_type_ = OH_ArkUI_NodeEvent_GetEventType(event_);
            node_handle_ = OH_ArkUI_NodeEvent_GetNodeHandle(event_);
        }

        bool TaroEventBase::realTrigger() {
            return true;
        }

        int TaroEventBase::parseHmEvent(ArkUI_NodeEvent* event) {
            return 0;
        }

        int TaroEventBase::serializeFun(napi_value& ret_val) {
            TARO_LOG_DEBUG("Event", "%{public}s", "EventRecallDataBase now");
            NapiSetter::SetProperty(ret_val, "type", js_event_type_);

            return 0;
        }

        int TaroEventBase::serializeFun() {
            napi_value event_obj = GetNapiValue();
            if (event_obj == nullptr) {
                return -1;
            }
            serializeFun(event_obj);
            return 0;
        }

        napi_value& TaroEventBase::detail() {
            if (js_detail_ == nullptr) {
                napi_create_object(NativeNodeApi::env, &js_detail_);
            }
            return js_detail_;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
