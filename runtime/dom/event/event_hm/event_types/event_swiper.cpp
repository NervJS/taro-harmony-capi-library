//
// Created on 2024/6/15.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "event_swiper.h"

#include "helper/TaroLog.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/element/swiper.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroElementChangedInSwiper::TaroElementChangedInSwiper(
            const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
            bubbles_ = false;
        }

        TaroElementChangedInSwiper::~TaroElementChangedInSwiper() {}

        int TaroElementChangedInSwiper::parseHmEvent(ArkUI_NodeEvent *event)  {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get swiper element change event failed");
                return -1;
            }
            current_ = component_event->data[0].i32;
            return 0;
        }

        int TaroElementChangedInSwiper::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            // detail填充
            auto &js_detail = detail();
            NapiSetter::SetProperty(js_detail, "current", current_);
            NapiSetter::SetProperty(js_detail, "source", "not support");
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        TaroEventTransitionInSwiper::TaroEventTransitionInSwiper(
            const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventTransitionInSwiper::parseHmEvent(ArkUI_NodeEvent *event) {
            auto component_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (component_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get swiper transition event failed");
                return -1;
            }
            move_val_ = component_event->data[2].f32;
            return 0;
        }

        int TaroEventTransitionInSwiper::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            // detail填充
            auto &js_detail = detail();
            if (auto swiperEl = std::dynamic_pointer_cast<TaroSwiper>(target_)) {
                auto swiperNode = swiperEl->GetHeadRenderNode();
                auto context = swiperNode->GetDimensionContext();
                if (swiperEl->GetVertical()) {
                    float swiperHeight = swiperNode->layoutDiffer_.computed_style_.height - (swiperEl->GetPrevMargin().ConvertToVp(context) + swiperEl->GetPrevMargin().ConvertToVp(context));
                    NapiSetter::SetProperty(js_detail, "dy", move_val_ * swiperHeight);
                } else {
                    float swiperWidth = swiperNode->layoutDiffer_.computed_style_.width - (swiperEl->GetPrevMargin().ConvertToVp(context) + swiperEl->GetPrevMargin().ConvertToVp(context));
                    NapiSetter::SetProperty(js_detail, "dx", move_val_ * swiperWidth);
                }
            }
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime