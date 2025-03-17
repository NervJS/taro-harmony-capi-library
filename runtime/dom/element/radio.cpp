//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "radio.h"

#include <arkui/native_node.h>

#include "runtime/NapiSetter.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/element/form/group_manager.h"
#include "runtime/dom/element/text.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_hm/event_types/event_type.h"
#include "runtime/dom/event/event_user/user_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroRadio::TaroRadio(napi_value node)
        : FormWidget(node),
          default_width_(Dimension{1, DimensionUnit::PERCENT}),
          default_height_(16),
          attributes_(std::make_unique<TaroRadioAttributes>()) {
        OH_Drawing_Font* font = OH_Drawing_FontCreate();
        // 获取单个文本的宽度
        if (OH_Drawing_FontMeasureText(font, "a", 1, TEXT_ENCODING_UTF8, nullptr, &textWidth_) != OH_DRAWING_SUCCESS) {
            textWidth_ = 16.0; // 设置默认的值
        }
        OH_Drawing_FontDestroy(font);
    }

    void TaroRadio::handleEvent() {
        auto onChange = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            if (parent_ != nullptr) {
                callJSFunc("change", parent_);
            } else {
                callJSFunc("change", shared_from_this());
            }
            return 0;
        };
        auto onClick = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            ChangeSelectState();
            return 0;
        };

        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_RADIO_ON_CHANGE, "change", onChange, radio_->GetArkUINodeHandle());
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", onClick, GetHeadRenderNode()->GetArkUINodeHandle());
    }

    void TaroRadio::callJSFunc(std::string eventType, std::shared_ptr<TaroNode> node) {
        auto event = std::make_shared<TaroEvent::UserEventBase>(eventType);
        napi_value js_detail = event->detail();
        NapiSetter::SetProperty(js_detail, "value", attributes_->value.value());
        auto elem = std::dynamic_pointer_cast<TaroElement>(node);
        elem->getEventEmitter()->triggerEvents(event);
    }

    void TaroRadio::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            radio_ = std::make_shared<TaroRadioNode>(element);
            radio_->Build();
            auto render_stack = std::make_shared<TaroStackNode>(element);
            render_stack->Build();
            SetHeadRenderNode(render_stack);
            SetFooterRenderNode(render_stack);
            render_stack->AppendChild(radio_); // FIXME 可能会导致子节点排序错误
        }
    }

    void TaroRadio::SetGroupInfo() {
        TaroHelper::Optional<std::string> groupName = attributes_->name;
        if (!groupName.has_value()) {
            if (parent_ == nullptr) {
                parent_ = getParentNode(TAG_NAME::RADIO_GROUP);
            }
            if (parent_) {
                groupName.set(std::to_string(parent_->nid_));
            }
        }
        if (groupName.has_value()) {
            radio_->setNodeGroup(groupName.value().c_str());
        } else {
            radio_->setNodeGroup(std::to_string(nid_).c_str());
        }
    }

    void TaroRadio::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();
        SetGroupInfo();
        auto new_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        std::shared_ptr<TaroStackNode> render_stack =
            std::static_pointer_cast<TaroStackNode>(GetFooterRenderNode());

        if (!new_style->height.has_value()) {
            new_style->height.set(default_height_);
        }

        if (!new_style->width.has_value()) {
            new_style->width.set(default_height_);
        }
        new_style->alignSelf.set(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_CENTER);
        new_style->marginRight.set(textWidth_); // 一个字符大小

        radio_->SetStyle(new_style);

        new_style->display.set(PropertyType::Display::Flex);

        new_style->width.set(default_width_);

        render_stack->SetStyle(new_style);
        handleEvent();
        SetCheckedAttribute();
        SetColorAttribute();
    }

    void TaroRadio::GetColorAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<std::string> color = getter.String();
        if (color.has_value()) {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor(color.value());
            if (colorValue.has_value()) {
                attributes_->color.set(colorValue.value());
            }
        }
    }
    void TaroRadio::GetCheckedAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<bool> checked = getter.Bool();
        if (checked.has_value()) {
            attributes_->checked.set(checked.value());
        }
    }

    void TaroRadio::ChangeSelectState() {
        std::shared_ptr<TaroRadioNode> render_radio = std::static_pointer_cast<TaroRadioNode>(radio_);
        render_radio->ChangeChecked();
    }

    void TaroRadio::SetCheckedAttribute() {
        if (attributes_->checked.has_value()) {
            std::shared_ptr<TaroRadioNode> render_radio = std::static_pointer_cast<TaroRadioNode>(radio_);
            render_radio->setChecked(attributes_->checked.value());
            if (attributes_->checked.value()) {
                if (parent_ != nullptr) {
                    callJSFunc("change", parent_);
                } else {
                    callJSFunc("change", shared_from_this());
                }
            }
        }
    }

    void TaroRadio::SetColorAttribute() {
        std::shared_ptr<TaroRadioNode> render_radio = std::static_pointer_cast<TaroRadioNode>(radio_);
        if (attributes_->color.has_value()) {
            std::shared_ptr<TaroRadioNode> render_radio = std::static_pointer_cast<TaroRadioNode>(radio_);
            render_radio->setColor(attributes_->color.value());
        } else {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor("#09BB07");
            render_radio->setColor(colorValue.value());
        }
    }

    void TaroRadio::SetValueAttribute() {
        if (attributes_->value.has_value()) {
            std::shared_ptr<TaroRadioNode> render_radio = std::static_pointer_cast<TaroRadioNode>(radio_);
            render_radio->setValue(attributes_->value.value().c_str());
        }
    }

    void TaroRadio::AddToGroupManager(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        TaroHelper::Optional<std::string> id = getter.String();
        if (id.has_value()) {
            if (context_) {
                if (context_->group_manger_ == nullptr) {
                    context_->group_manger_ = std::make_shared<FormGroupManager>();
                }
                context_->group_manger_->AppendChildWithID(id.value(), shared_from_this());
            }
        }
    }

    void TaroRadio::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
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
            default:
                break;
        }
    }

    void TaroRadio::SetParent(const std::shared_ptr<TaroNode>& node) {
        parent_ = node;
        if (is_init_) {
            SetGroupInfo();
        }
    }

    void TaroRadio::HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) {
        FormWidget::HandleAttributeChanged(name, preValue, curValue);
        switch (name) {
            case ATTRIBUTE_NAME::ID:
                ChangeIDAttribute(preValue, curValue);
                break;
            default:
                break;
        }
    }

    void TaroRadio::ChangeIDAttribute(const std::string& preValue, const std::string& curValue) {
        if (context_) {
            if (context_->group_manger_ == nullptr) {
                context_->group_manger_ = std::make_shared<FormGroupManager>();
            } else {
                context_->group_manger_->DelChildForGroup(preValue, shared_from_this());
            }
            context_->group_manger_->AppendChildWithID(curValue, shared_from_this());
        }
    }

    bool TaroRadio::bindListenEvent(const std::string& event_name) {
        if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else {
            return false;
        }
        return true;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
