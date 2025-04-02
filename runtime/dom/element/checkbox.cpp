/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "checkbox.h"

#include <native_drawing/drawing_font.h>

#include "runtime/NapiSetter.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/checkbox.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/element/checkbox_group.h"
#include "runtime/dom/element/form/group_manager.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_hm/event_types/event_checkbox.h"
#include "runtime/dom/event/event_hm/event_types/event_type.h"
#include "runtime/dom/event/event_user/user_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroCheckbox::TaroCheckbox(napi_value node)
        : FormWidget(node),
          default_width_(Dimension{1, DimensionUnit::PERCENT}),
          default_height_(16),
          attributes_(std::make_unique<TaroCheckboxAttributes>()) {
        OH_Drawing_Font* font = OH_Drawing_FontCreate();
        // 获取单个文本的宽度
        if (OH_Drawing_FontMeasureText(font, "a", 1, TEXT_ENCODING_UTF8, nullptr, &textWidth_) != OH_DRAWING_SUCCESS) {
            textWidth_ = 16.0; // 设置默认的值
        }
        OH_Drawing_FontDestroy(font);
    }

    void TaroCheckbox::handleEvent() {
        auto onChange = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto change_event = std::static_pointer_cast<TaroEvent::TaroCheckboxEventChange>(event);
            if (parent_ != nullptr) {
                auto group = std::static_pointer_cast<TaroCheckboxGroup>(parent_);
                group->callEventHandler(attributes_->value.value(), change_event->selected_value_);
            } else {
                callJSFunc("change", change_event->selected_value_);
            }
            return 0;
        };

        auto onClick = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            ChangeSelectState();
            return 0;
        };

        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CHECKBOX_ON_CHANGE, "change", onChange, checkbox_->GetArkUINodeHandle());
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", onClick, GetHeadRenderNode()->GetArkUINodeHandle());
    }

    void TaroCheckbox::callJSFunc(std::string eventType, int32_t select) {
        auto event = std::make_shared<TaroEvent::UserEventBase>(eventType);
        napi_value js_detail = event->detail();
        std::set<std::string> array_value;
        if (select) {
            array_value.emplace(attributes_->value.value());
        }
        NapiSetter::SetProperty(js_detail, "value", array_value);
        getEventEmitter()->triggerEvents(event);
    }

    bool TaroCheckbox::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "focus") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_FOCUS, event_name);
        } else if (event_name == "blur") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_BLUR, event_name);
        } else if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroCheckbox::Build() {
        if (!is_init_) {
            // 构建checkbox render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            checkbox_ = std::make_shared<TaroCheckboxNode>(element);
            checkbox_->Build();
            auto render_stack = std::make_shared<TaroStackNode>(element);
            render_stack->Build();

            SetRenderNode(render_stack);

            SetColorAttribute();
            SetShapeAttribute();

            render_stack->AppendChild(checkbox_);

            parent_ = getParentNode(TAG_NAME::CHECKBOX_GROUP);
        }
        handleEvent();
    }

    void TaroCheckbox::GetColorAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<std::string> color = getter.String();
        if (color.has_value()) {
            attributes_->color.set(color.value());
        }
    }

    void TaroCheckbox::GetCheckedAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<bool> checked = getter.Bool();
        if (checked.has_value()) {
            attributes_->checked.set(checked.value());
        }
    }

    void TaroCheckbox::GetShapeAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<std::string> shape = getter.String();
        if (shape.has_value()) {
            attributes_->shape.set(shape.value());
        }
    }

    void TaroCheckbox::SetCheckedAttribute() {
        if (attributes_->checked.has_value()) {
            std::shared_ptr<TaroCheckboxNode> render_checkbox = std::static_pointer_cast<TaroCheckboxNode>(checkbox_);
            render_checkbox->SetChecked(attributes_->checked.value());
            if (parent_ != nullptr) {
                auto group = std::static_pointer_cast<TaroCheckboxGroup>(parent_);
                group->callEventHandler(attributes_->value.value(), attributes_->checked.value());
            } else {
                callJSFunc("change", int(attributes_->checked.value()));
            }
        }
    }

    void TaroCheckbox::SetColorAttribute() {
        std::shared_ptr<TaroCheckboxNode> render_checkbox = std::static_pointer_cast<TaroCheckboxNode>(checkbox_);
        if (attributes_->color.has_value()) {
            auto color = TaroHelper::StringUtils::parseColor(attributes_->color.value());
            if (color.value()) {
                render_checkbox->SetColor(color.value());
                return;
            }
        }
        std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor("#09BB07");
        render_checkbox->SetColor(colorValue.value());
    }

    void TaroCheckbox::SetShapeAttribute() {
        std::shared_ptr<TaroCheckboxNode> render_checkbox = std::static_pointer_cast<TaroCheckboxNode>(checkbox_);
        if (attributes_->shape.has_value()) {
            render_checkbox->SetShape(attributes_->shape.value().c_str());
        } else {
            render_checkbox->SetShape("square");
        }
    }

    void TaroCheckbox::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        FormWidget::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::COLOR:
                GetColorAttribute(value);
                if (is_init_) {
                    SetColorAttribute();
                }
                break;
            case ATTRIBUTE_NAME::CHECKED:
                GetCheckedAttribute(value);
                if (is_init_) {
                    SetCheckedAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SHAPE:
                GetShapeAttribute(value);
                if (is_init_) {
                    SetShapeAttribute();
                }
                break;
            default:
                break;
        }
    }

    void TaroCheckbox::ChangeSelectState() {
        std::shared_ptr<TaroCheckboxNode> render_checkbox = std::static_pointer_cast<TaroCheckboxNode>(checkbox_);
        render_checkbox->ChangeChecked();
    }

    void TaroCheckbox::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();
        if (attributes_->name.has_value()) {
            parent_ = nullptr;
        }

        auto stack_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        auto checkbox_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        std::shared_ptr<TaroStackNode> render_stack =
            std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());

        if (!stack_style->height.has_value()) {
            stack_style->height.set(default_height_);
        }

        if (!stack_style->width.has_value()) {
            stack_style->width.set(default_width_);
        }

        stack_style->justifyContent.set(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_CENTER);
        stack_style->alignItems.set(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_START);
        stack_style->display.set(PropertyType::Display::Flex);

        render_stack->SetStyle(stack_style);
        render_stack->SetAlign(ArkUI_Alignment::ARKUI_ALIGNMENT_CENTER);

        if (!checkbox_style->height.has_value()) {
            checkbox_style->height.set(default_height_);
        }

        checkbox_style->width.set(checkbox_style->height);
        checkbox_style->marginRight.set(textWidth_); // 一个字符大小
        checkbox_->SetStyle(checkbox_style);

        SetCheckedAttribute();
        SetColorAttribute();
    }

    void TaroCheckbox::SetLink() {
        link = true;
    }

    void TaroCheckbox::SetParent(const std::shared_ptr<TaroNode>& node) {
        parent_ = node;
    }

    void TaroCheckbox::HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) {
        FormWidget::HandleAttributeChanged(name, preValue, curValue);
        switch (name) {
            case ATTRIBUTE_NAME::ID:
                ChangeIDAttribute(preValue, curValue);
                break;
            default:
                break;
        }
    }

    void TaroCheckbox::ChangeIDAttribute(const std::string& preValue, const std::string& curValue) {
        if (context_) {
            if (context_->group_manger_ == nullptr) {
                context_->group_manger_ = std::make_shared<FormGroupManager>();
            } else {
                context_->group_manger_->DelChildForGroup(preValue, shared_from_this());
            }
            context_->group_manger_->AppendChildWithID(curValue, shared_from_this());
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
