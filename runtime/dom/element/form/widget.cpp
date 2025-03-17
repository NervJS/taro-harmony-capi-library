//
// Created by zhutianjian on 24-8-31.
//

#include "widget.h"

#include "arkjs/ArkJS.h"
#include "runtime/dom/event/event_hm/event_types/event_change.h"

namespace TaroRuntime {
namespace TaroDOM {
    std::unordered_map<std::string, FORM_FUNC_CODE> FORM_FUNC_CODE_MAPPING = {
        {"focus", FORM_FUNC_CODE::FOCUS},
        {"blur", FORM_FUNC_CODE::BLUR},
    };

    FormWidget::FormWidget(napi_value node)
        : TaroElement(node) {
    }

    void FormWidget::PostBuild() {
        if (!is_init_) {
            bindSelfEventHandle();
        }

        TaroElement::PostBuild();
    }

    void FormWidget::bindSelfEventHandle() {
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_FOCUS, "focus", [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            is_focus = true;
            return 0;
        });
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_BLUR, "blur", [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            is_focus = false;
            return 0;
        });
    }

    void FormWidget::GetNodeAttributes() {
        TaroElement::GetNodeAttributes();

        GetNameAttribute(GetAttributeNodeValue("name"));
        GetValueAttribute(GetAttributeNodeValue("value"));
    }

    void FormWidget::GetNameAttribute(const napi_value& napiValue) {
        ArkJS arkJs(NativeNodeApi::env);
        if (napiValue && arkJs.getType(napiValue) == napi_string) {
            GetAttrs()->name.set(arkJs.getString(napiValue));
        }
    }

    void FormWidget::GetValueAttribute(const napi_value& napiValue) {
        ArkJS arkJs(NativeNodeApi::env);
        if (napiValue && arkJs.getType(napiValue) == napi_string) {
            GetAttrs()->value.set(arkJs.getString(napiValue));
        }
    }

    void FormWidget::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroElement::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::NAME:
                GetNameAttribute(value);
                break;
            case ATTRIBUTE_NAME::VALUE: {
                GetValueAttribute(value);
                if (is_init_) {
                    SetValueAttribute();
                }
                break;
            }
            default:
                break;
        }
    }

    void FormWidget::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        SetValueAttribute();
    }

    std::variant<bool, int, std::string, StylesheetRef, napi_value> FormWidget::GetAttribute(ATTRIBUTE_NAME name, napi_value value) {
        auto attr = GetAttrs();
        switch (name) {
            case ATTRIBUTE_NAME::NAME:
                if (attr->name.has_value()) {
                    return attr->name.value();
                }
                break;
            case ATTRIBUTE_NAME::VALUE:
                if (attr->value.has_value()) {
                    return attr->value.value();
                }
                break;
            default:
                break;
        }
        return TaroElement::GetAttribute(name, value);
    }

    int FormWidget::GetFuncCode(std::string str) {
        auto it = FORM_FUNC_CODE_MAPPING.find(str);
        if (it != FORM_FUNC_CODE_MAPPING.end()) {
            return static_cast<int>(it->second);
        }
        return TaroElement::GetFuncCode(str);
    }

    napi_value FormWidget::ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params) {
        // auto node = std::static_pointer_cast<FormWidget>(renderNode);
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        FORM_FUNC_CODE code = static_cast<FORM_FUNC_CODE>(TaroElement::GetFuncCode(name));
        switch (code) {
            case FORM_FUNC_CODE::FOCUS: {
                ArkUI_NumberValue value = {.i32 = 1};
                ArkUI_AttributeItem item = {&value, 1};
                nativeNodeApi->setAttribute(renderNode->GetArkUINodeHandle(), NODE_FOCUS_STATUS, &item);
                return nullptr;
            }
            case FORM_FUNC_CODE::BLUR: {
                ArkUI_NumberValue value = {.i32 = 0};
                ArkUI_AttributeItem item = {&value, 1};
                nativeNodeApi->setAttribute(renderNode->GetArkUINodeHandle(), NODE_FOCUS_STATUS, &item);
                return nullptr;
            }
            default:
                return TaroElement::ExecuteFunc(renderNode, name, params);
        }
        return nullptr;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
