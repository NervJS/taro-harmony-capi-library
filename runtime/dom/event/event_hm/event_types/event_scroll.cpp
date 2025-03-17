//
// Created on 2024/4/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_scroll.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventScroll::TaroEventScroll(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventScroll::parseHmEvent(ArkUI_NodeEvent *event) {
            auto comp_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (comp_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get comp_event failed");
                return -1;
            }
            deltaX_ = comp_event->data[0].f32;
            deltaY_ = comp_event->data[1].f32;
            return 0;
        }

        int TaroEventScroll::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            // detail填充
            auto js_detail = detail();
            // 表示距离上一次事件触发的X轴增量。
            NapiSetter::SetProperty(js_detail, "deltaX", deltaX_);
            // 表示距离上一次事件触发的Y轴增量。
            NapiSetter::SetProperty(js_detail, "deltaY", deltaY_);
            // isDrag、scrollTop、scrollLeft、scrollHeight、scrollWeight需要从node上获取

            NapiSetter::SetProperty(ret_obj, "detail", js_detail);

            return 0;
        }

        TaroEventDidScroll::TaroEventDidScroll(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventDidScroll::parseHmEvent(ArkUI_NodeEvent *event) {
            auto comp_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (comp_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get comp_event failed");
                return -1;
            }
            deltaX_ = comp_event->data[0].f32;
            deltaY_ = comp_event->data[1].f32;
            scrollState_ = (ArkUI_ScrollState)comp_event->data[2].i32;
            return 0;
        }

        int TaroEventDidScroll::serializeFun(napi_value &ret_obj) {
            // detail填充
            auto &js_detail = detail();
            // 表示距离上一次事件触发的X轴增量。
            NapiSetter::SetProperty(js_detail, "deltaX", deltaX_);
            // 表示距离上一次事件触发的Y轴增量。
            NapiSetter::SetProperty(js_detail, "deltaY", deltaY_);
            // isDrag、scrollTop、scrollLeft、scrollHeight、scrollWeight需要从node上获取

            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        TaroEventDidWaterFlowScroll::TaroEventDidWaterFlowScroll(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventDidWaterFlowScroll::parseHmEvent(ArkUI_NodeEvent *event)  {
            auto comp_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (comp_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get comp_event failed");
                return -1;
            }
            deltaY_ = comp_event->data[0].f32;
            scrollState_ = (ArkUI_ScrollState)comp_event->data[1].i32;
            return 0;
        }

        int TaroEventDidWaterFlowScroll::serializeFun(napi_value &ret_obj) {
            // detail填充
            auto &js_detail = detail();
            // 表示距离上一次事件触发的Y轴增量。
            NapiSetter::SetProperty(js_detail, "deltaY", deltaY_);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        TaroEventLazyLoadScrollIndex::TaroEventLazyLoadScrollIndex(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {
        }

        int TaroEventLazyLoadScrollIndex::parseHmEvent(ArkUI_NodeEvent *event) {
            auto comp_event = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
            if (comp_event == nullptr) {
                TARO_LOG_DEBUG("TaroEvent", "get comp_event failed");
                return -1;
            }
            startIndex_ = comp_event->data[0].i32;
            endIndex_ = comp_event->data[1].i32;
            return 0;
        }

        int TaroEventLazyLoadScrollIndex::serializeFun(napi_value &ret_obj) {
            // detail填充
            auto &js_detail = detail();
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
