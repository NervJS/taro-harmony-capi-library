//
// Created on 2024/4/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_touch.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventTouch::TaroEventTouch(const std::string &js_event_type, ArkUI_NodeEvent *event)
                : TaroEventBase(js_event_type, event) {
        }

        bool TaroEventTouch::realTrigger() {
            return hm_touch_type_ == event_touch_type;
        }

        void TaroEventTouch::revertEventBubble () {
            auto event = getOriginEvent();
            auto input_event = OH_ArkUI_NodeEvent_GetInputEvent(event);
            if (input_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get touch_action failed");
                return;
            }
            OH_ArkUI_PointerEvent_SetStopPropagation(input_event, false);
        }

        int TaroEventTouch::parseHmEvent(ArkUI_NodeEvent *event) {
            auto input_event = OH_ArkUI_NodeEvent_GetInputEvent(event);
            if (input_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get touch_action failed");
                return -1;
            }
            OH_ArkUI_PointerEvent_SetStopPropagation(input_event, true);
            event_touch_type= OH_ArkUI_UIInputEvent_GetAction(input_event);
            client_x_ = OH_ArkUI_PointerEvent_GetX(input_event);
            client_y_ = OH_ArkUI_PointerEvent_GetY(input_event);
            page_x_ = OH_ArkUI_PointerEvent_GetWindowX(input_event);
            page_y_ = OH_ArkUI_PointerEvent_GetWindowY(input_event);
            screen_x_ = OH_ArkUI_PointerEvent_GetDisplayX(input_event);
            screen_y_ = OH_ArkUI_PointerEvent_GetDisplayY(input_event);
            return 0;
        }

        int TaroEventTouch::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            // TARO_LOG_DEBUG("TaroEvent", "touch: %{public}d", hm_touch_type_);

            // changedTouches填充
            napi_value napi_changed_touches;
            napi_create_array(NativeNodeApi::env, &napi_changed_touches);

            napi_value napi_changed_touches_elem;
            napi_create_object(NativeNodeApi::env, &napi_changed_touches_elem);
            // 点击位置相对于被点击元素原始区域左上角的X坐标，单位vp
            NapiSetter::SetProperty(napi_changed_touches_elem, "clientX", client_x_);
            // 点击位置相对于被点击元素原始区域左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "clientY", client_y_);
            // 点击位置相对于应用窗口左上角的X坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "pageX", page_x_);
            // 点击位置相对于应用窗口左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "pageY", page_y_);
            // 点击位置相对于应用屏幕左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "screenX", screen_x_);
            // 点击位置相对于应用屏幕左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "screenY", screen_y_);

            // 触摸点的唯一标识符。
            // NapiSetter::SetProperty(napi_changed_touches_elem, "identifier", event_->touchEvent.actionTouch.id);
            NapiSetter::SetProperty(napi_changed_touches_elem, "identifier", 0);
            // 触摸点施加的力的大小（仅在某些支持力感应的设备上可用）。
            napi_set_element(NativeNodeApi::env, napi_changed_touches, 0, napi_changed_touches_elem);
            NapiSetter::SetProperty(ret_obj, "changedTouches", napi_changed_touches);
            NapiSetter::SetProperty(ret_obj, "touches", napi_changed_touches);
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
