/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_click.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        int TaroEventClick::parseHmEvent(ArkUI_NodeEvent* event) {
            auto com_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (com_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get touch_action failed");
                return -1;
            }
            x_ = com_event->data[0].f32;
            y_ = com_event->data[1].f32;
            client_x_ = com_event->data[6].f32;
            client_y_ = com_event->data[7].f32;
            page_x_ = com_event->data[4].f32;
            page_y_ = com_event->data[5].f32;
            screen_x_ = com_event->data[6].f32;
            screen_y_ = com_event->data[7].f32;
            return 0;
        }

        int TaroEventClick::serializeFun(napi_value& ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            // changedTouches填充
            napi_value napi_changed_touches;
            napi_create_array(NativeNodeApi::env, &napi_changed_touches);

            napi_value napi_changed_touches_elem;
            napi_create_object(NativeNodeApi::env, &napi_changed_touches_elem);
            // 点击位置相对于被点击元素原始区域左上角的X坐标，单位vp
            NapiSetter::SetProperty(napi_changed_touches_elem, "clientX",
                                    client_x_);
            // 点击位置相对于被点击元素原始区域左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "clientY",
                                    client_y_);
            // 点击位置相对于应用窗口左上角的X坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "pageX",
                                    page_x_);
            // 点击位置相对于应用窗口左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "pageY",
                                    page_y_);
            // 点击位置相对于应用屏幕左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "screenX",
                                    screen_x_);
            // 点击位置相对于应用屏幕左上角的Y坐标，单位vp。
            NapiSetter::SetProperty(napi_changed_touches_elem, "screenY",
                                    screen_y_);
            // 触摸点的唯一标识符。
            NapiSetter::SetProperty(napi_changed_touches_elem, "identifier", 0);
            // 触摸点施加的力的大小（仅在某些支持力感应的设备上可用）。
            //  NapiSetter::SetProperty(napi_changed_touches_elem, "force", event_->componentEvent.data[7].f32);
            napi_set_element(NativeNodeApi::env, napi_changed_touches, 0, napi_changed_touches_elem);
            NapiSetter::SetProperty(ret_obj, "changedTouches", napi_changed_touches);

            // detail填充
            napi_value js_detail = detail();
            // 点击事件的detail 带有的 x, y 同 pageX, pageY 代表距离文档左上角的距离
            NapiSetter::SetProperty(js_detail, "x", x_);
            NapiSetter::SetProperty(js_detail, "y", y_);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);

            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
