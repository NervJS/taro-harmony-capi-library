//
// Created on 2024/8/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_slider.h"

#include "helper/TaroLog.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        int TaroSliderEventChange::parseHmEvent(ArkUI_NodeEvent *event) {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get component_event failed");
                return -1;
            }

            value_ = component_event->data[0].f32;
            state_ = component_event->data[1].i32;
            return 0;
        }

        const float &TaroSliderEventChange::getValue() {
            return value_;
        }

        int TaroSliderEventChange::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            // detail填充
            napi_value js_detail = detail();
            NapiSetter::SetProperty(js_detail, "value", value_);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        bool TaroSliderEventChange::realTrigger() {
            return state_ == 2;
        }

        bool TaroSliderEventChanging::realTrigger() {
            return state_ != 2;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
