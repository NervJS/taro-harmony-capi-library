//
// Created on 2024/6/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_input.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroInputFocus::TaroInputFocus(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {}

        TaroInputFocus::~TaroInputFocus() {}

        int TaroInputFocus::parseHmEvent(ArkUI_NodeEvent *event) {
            auto arkui_node = OH_ArkUI_NodeEvent_GetNodeHandle(event);
            // The pointer returned by this API is an internal buffer pointer of the ArkUI framework.
            // As such, you do not need to call delete to release the memory.
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            const ArkUI_AttributeItem *item = nativeNodeApi->getAttribute(arkui_node, NODE_TEXT_INPUT_TEXT);
            if (item != nullptr && item->string != nullptr) {
                value_ = item->string;
            } else {
                value_ = "not support";
            }
            return 0;
        }

        int TaroInputFocus::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            // detail填充
            auto &js_detail = detail();
            NapiSetter::SetProperty(js_detail, "value", value_);
            NapiSetter::SetProperty(js_detail, "height", 0);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        TaroInputBlur::TaroInputBlur(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {}

        TaroInputBlur::~TaroInputBlur() {}

        int TaroInputBlur::parseHmEvent(ArkUI_NodeEvent *event) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            auto arkui_node = OH_ArkUI_NodeEvent_GetNodeHandle(event);
            // The pointer returned by this API is an internal buffer pointer of the ArkUI framework.
            // As such, you do not need to call delete to release the memory.
            const ArkUI_AttributeItem *item = nativeNodeApi->getAttribute(arkui_node, NODE_TEXT_INPUT_TEXT);
            if (item != nullptr && item->string != nullptr) {
                value_ = item->string;
            } else {
                value_ = "not support";
            }
            return 0;
        }

        int TaroInputBlur::serializeFun(napi_value &ret_obj) {

            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            // detail填充
            auto &js_detail = detail();
            NapiSetter::SetProperty(js_detail, "value", value_);
            NapiSetter::SetProperty(js_detail, "height", 0);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }

        TaroInputSubmit::TaroInputSubmit(const std::string &js_event_type, ArkUI_NodeEvent *event)
            : TaroEventBase(js_event_type, event) {}

        TaroInputSubmit::~TaroInputSubmit() {}

        int TaroInputSubmit::parseHmEvent(ArkUI_NodeEvent *event) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            auto arkui_node = OH_ArkUI_NodeEvent_GetNodeHandle(event);
            // The pointer returned by this API is an internal buffer pointer of the ArkUI framework.
            // As such, you do not need to call delete to release the memory.
            const ArkUI_AttributeItem *item = nativeNodeApi->getAttribute(arkui_node, NODE_TEXT_INPUT_TEXT);
            if (item != nullptr && item->string != nullptr) {
                value_ = item->string;
            } else {
                value_ = "not support";
            }
            return 0;
        }

        int TaroInputSubmit::serializeFun(napi_value &ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);
            // detail填充
            auto &js_detail = detail();
            NapiSetter::SetProperty(js_detail, "value", value_);
            NapiSetter::SetProperty(js_detail, "height", 0);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime