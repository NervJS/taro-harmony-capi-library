/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_change.h"

#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/element/input.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        int TaroTextInputEventChange::parseHmEvent(ArkUI_NodeEvent* event) {
            if (value_.empty()) {
                ArkUI_StringAsyncEvent* ptr_ev_string = OH_ArkUI_NodeEvent_GetStringAsyncEvent(event);
                if (ptr_ev_string != nullptr && ptr_ev_string->pStr != nullptr) {
                    value_ = ptr_ev_string->pStr;
                }
            }
            return 0;
        }

        const std::string& TaroTextInputEventChange::getValue() {
            return value_;
        }

        int TaroTextInputEventChange::serializeFun(napi_value& ret_obj) {
            NapiSetter::SetProperty(ret_obj, "type", js_event_type_);

            napi_value js_detail = detail();
            std::string value = getValue();
            NapiSetter::SetProperty(js_detail, "value", value);
            NapiSetter::SetProperty(ret_obj, "detail", js_detail);

            return 0;
        }

        bool TaroTextInputEventInput::realTrigger() {
            if (auto input = std::static_pointer_cast<TaroInput>(target_)) {
                auto attrValue = input->GetAttrs()->value;
                // Note: 值无变化或用户未选中时，不触发 Input 事件
                if (getValue() == input->pre_value || !input->is_focus) {
                    return false;
                }
            }
            return true;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
