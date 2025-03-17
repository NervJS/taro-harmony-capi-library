//
// Created on 2024/6/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "input.h"

#include <boost/algorithm/string.hpp>

#include "helper/TaroLog.h"
#include "runtime/dom/ark_nodes/text_area.h"
#include "runtime/dom/event/event_hm/event_types/event_change.h"
#include "runtime/dom/event/event_visible.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroInput::TaroInput(napi_value node)
        : FormWidget(node) {
        if (tag_name_ == TAG_NAME::INPUT) {
            attributes_ = std::make_unique<TaroInputAttributes>();
        }
    }

    void TaroInput::Build() {
        if (!is_init_) {
            // create render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_input = std::make_shared<TaroTextInputNode>(element);
            render_input->Build();

            SetRenderNode(render_input);
        }
    }

    bool TaroInput::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "focus") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_FOCUS, event_name);
        } else if (event_name == "blur") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_BLUR, event_name);
        } else if (event_name == "input") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_INPUT_ON_INPUT, event_name);
        } else if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_INPUT_ON_CHANGE, event_name);
        } else if (event_name == "confirm") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_INPUT_ON_CONFIRM, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroInput::bindSelfEventHandle() {
        FormWidget::bindSelfEventHandle();
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_INPUT_ON_CHANGE, "change", [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            if (auto change = std::dynamic_pointer_cast<TaroEvent::TaroTextInputEventChange>(event)) {
                std::string value = change->getValue();
                auto attr = GetAttrs();
                if (attr->value.has_value()) {
                    pre_value = std::move(attr->value.value());
                }
                attr->value.set(value);
            }
            return 0;
        });
    }

    void TaroInput::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        switch (name) {
            case ATTRIBUTE_NAME::PASSWORD: {
                GetPasswordAttribute(value);
                if (is_init_) {
                    SetPasswordAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::TYPE: {
                GetTypeAttribute(value);
                if (is_init_) {
                    SetTypeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::CONFIRM_TYPE: {
                GetConfirmTypeAttribute(value);
                if (is_init_) {
                    SetConfirmTypeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::MAX_LENGTH: {
                GetMaxLengthTypeAttribute(value);
                if (is_init_) {
                    SetMaxLengthTypeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::FOCUS: {
                GetFocusTypeAttribute(value);
                if (is_init_) {
                    SetFocusTypeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::PLACEHOLDER: {
                GetPlaceholderAttribute(value);
                if (is_init_) {
                    SetPlaceholderAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::PLACEHOLDER_STYLE: {
                GetPlaceholderStyleAttribute(value);
                if (is_init_) {
                    SetPlaceholderStyleAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::PLACEHOLDER_TEXT_COLOR: {
                GetPlaceholderTextColorAttribute(value);
                if (is_init_) {
                    SetPlaceholderTextColorAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::ADJUST_POSITION: {
                GetAdjustPosition(value);
                if (is_init_) {
                    SetAdjustPosition();
                }
                break;
            }
            case ATTRIBUTE_NAME::EDITING: {
                GetEditingAttribute(value);
                if (is_init_) {
                    SetEditingAttribute();
                }
                break;
            }
            default:
                FormWidget::SetAttribute(renderNode, name, value);
                break;
        }
    }

    void TaroInput::SetValueAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->value.has_value()) {
            render_input->SetTextContent(GetAttrs()->value.value());
        }
    }

    void TaroInput::GetPasswordAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> password = getter.BoolNull();
        if (password.has_value()) {
            isPassword = password.value();
        }
    }

    void TaroInput::SetPasswordAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (isPassword) {
            render_input->SetInputType(ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_PASSWORD);
        } else {
            SetTypeAttribute();
        }
    }

    void TaroInput::GetTypeAttribute(const napi_value& value) {
        NapiGetter getter(value);
        auto type = static_cast<ArkUI_TextInputType>(GetInputType(getter.StringOr("")));
        GetAttrs()->type.set(type);
    }

    void TaroInput::SetTypeAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->type.has_value()) {
            render_input->SetInputType(GetAttrs()->type.value());
        }
    }

    void TaroInput::GetConfirmTypeAttribute(const napi_value& value) {
        NapiGetter getter(value);
        ArkUI_EnterKeyType type = GetConfirmType(getter.StringOr(""));
        GetAttrs()->confirmType.set(type);
    }

    void TaroInput::SetConfirmTypeAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->confirmType.has_value() && GetAttrs()->type.value() == ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_NORMAL) {
            render_input->SetConfirmType(GetAttrs()->confirmType.value());
        }
    }

    void TaroInput::GetMaxLengthTypeAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<int32_t> res = getter.Int32();
        if (res.has_value()) {
            GetAttrs()->maxLength.set(res.value());
        }
    }

    void TaroInput::SetMaxLengthTypeAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->maxLength.has_value()) {
            render_input->SetMaxLength(GetAttrs()->maxLength.value());
        }
    }

    void TaroInput::GetFocusTypeAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> res = getter.BoolNull();
        if (res.has_value()) {
            GetAttrs()->focus.set(res.value());
        }
    }

    void TaroInput::SetFocusTypeAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (is_appeared) {
            if (GetAttrs()->focus.has_value()) {
                render_input->SetAutoFocus(GetAttrs()->focus.value());
            }
        }
    }

    void TaroInput::GetPlaceholderAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> res = getter.String();
        if (res.has_value()) {
            GetAttrs()->placeholder.set(res.value());
        }
    }

    void TaroInput::SetPlaceholderAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->placeholder.has_value()) {
            render_input->SetPlaceholder(GetAttrs()->placeholder.value());
        }
    }

    void TaroInput::GetPlaceholderStyleAttribute(const napi_value& value) {
        NapiGetter getter(value);
        if (getter.String().has_value()) {
            TaroHelper::Optional<PlaceholderStyle> res = GetPlaceholderStyle(getter.String().value());
            GetAttrs()->placeholderStyle.set(res.value());
        }
    }

    void TaroInput::SetPlaceholderStyleAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->placeholderStyle.has_value()) {
            render_input->SetPlaceholderStyle(GetAttrs()->placeholderStyle.value());
        }
    }

    void TaroInput::GetPlaceholderTextColorAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> placeholderTextColor = getter.String();
        if (placeholderTextColor.has_value()) {
            std::optional<uint32_t> color = TaroHelper::StringUtils::parseColor(placeholderTextColor.value());
            if (color.has_value()) {
                GetAttrs()->placeholderTextColor.set(color.value());
            }
        }
    }

    void TaroInput::SetPlaceholderTextColorAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->placeholderTextColor.has_value()) {
            render_input->SetPlaceholderTextColor(GetAttrs()->placeholderTextColor.value());
        }
    }

    void TaroInput::GetAdjustPosition(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> res = getter.BoolNull();
        if (res.has_value()) {
            GetAttrs()->adjustPosition.set(res.value());
        }
    }

    void TaroInput::SetAdjustPosition() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        // Note: 该特性仅在文档显示可用，实测会收到 safeArea 影响
        render_input->SetCustomKeyBoard(GetAttrs()->adjustPosition.value());
    }

    void TaroInput::GetEditingAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> res = getter.BoolNull();
        if (res.has_value()) {
            GetAttrs()->editing.set(res.value());
        }
    }

    void TaroInput::SetEditingAttribute() {
        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        if (GetAttrs()->editing.has_value()) {
            render_input->SetEditing(GetAttrs()->editing.value());
        }
    }

    void TaroInput::GetNodeAttributes() {
        FormWidget::GetNodeAttributes();

        GetPasswordAttribute(GetAttributeNodeValue("password"));
        GetTypeAttribute(GetAttributeNodeValue("type"));
        GetConfirmTypeAttribute(GetAttributeNodeValue("confirmType"));
        GetMaxLengthTypeAttribute(GetAttributeNodeValue("maxLength"));
        GetFocusTypeAttribute(GetAttributeNodeValue("focus"));
        if (!GetAttrs()->focus.has_value()) {
            GetFocusTypeAttribute(GetAttributeNodeValue("autoFocus"));
        }
        GetPlaceholderAttribute(GetAttributeNodeValue("placeholder"));
        GetPlaceholderStyleAttribute(GetAttributeNodeValue("placeholderStyle"));
        GetPlaceholderTextColorAttribute(GetAttributeNodeValue("placeholderTextColor"));
    }

    void TaroInput::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();

        std::shared_ptr<TaroTextInputNode> render_input = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());

        SetPasswordAttribute();
        SetTypeAttribute();
        SetConfirmTypeAttribute();
        SetMaxLengthTypeAttribute();
        SetPlaceholderAttribute();
        SetPlaceholderStyleAttribute();
        SetPlaceholderTextColorAttribute();
    
        if (!is_appeared) {
            // 防止折叠屏重新buildProcess时，再次根据focus属性强制让input又聚焦了一次
            auto func = [this, render_input](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                is_appeared = true;
                SetFocusTypeAttribute();
                event_emitter_->unRegisterEventByName_NoCallBack("appear");
                return 0;
            };
            event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_NODE_APPEAR, "appear", func);
        }

        auto overrideStyle = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        overrideStyle->paddingTop.set(style_->paddingTop.value_or(0));
        overrideStyle->paddingRight.set(style_->paddingRight.value_or(0));
        overrideStyle->paddingBottom.set(style_->paddingBottom.value_or(0));
        overrideStyle->paddingLeft.set(style_->paddingLeft.value_or(0));
        overrideStyle->borderTopLeftRadius.set(style_->borderTopLeftRadius.value_or(0));
        overrideStyle->borderTopRightRadius.set(style_->borderTopRightRadius.value_or(0));
        overrideStyle->borderBottomRightRadius.set(style_->borderBottomRightRadius.value_or(0));
        overrideStyle->borderBottomLeftRadius.set(style_->borderBottomLeftRadius.value_or(0));

        // 默认值
        overrideStyle->width.set(style_->width.value_or(Dimension(1, DimensionUnit::PERCENT)));
        overrideStyle->height.set(style_->height.value_or(Dimension(40, DimensionUnit::VP)));
        overrideStyle->color.set(disabled_ ? 0xFF545454 : style_->color.value_or(0xFF000000));

        render_input->SetStyle(overrideStyle);
    }

    int TaroInput::GetInputType(const std::string& type) {
        if (isPassword) {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_PASSWORD;
        }
        if (type == "number" || type == "numberpad") {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_NUMBER;
        }
        if (type == "idcard" || type == "idcardpad") {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_NUMBER;
        }
        if (type == "digit" || type == "digitpad") {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_NUMBER_DECIMAL;
        }
        if (type == "safe-password") {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_PASSWORD;
        }
        if (type == "nickname") {
            return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_USER_NAME;
        }
        return ArkUI_TextInputType::ARKUI_TEXTINPUT_TYPE_NORMAL;
    }

    ArkUI_EnterKeyType TaroInput::GetConfirmType(const std::string& confirmType) {
        if (confirmType == "done") {
            return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_DONE;
        }
        if (confirmType == "send") {
            return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_SEND;
        }
        if (confirmType == "search") {
            return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_SEARCH;
        }
        if (confirmType == "next") {
            return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_NEXT;
        }
        if (confirmType == "go") {
            return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_GO;
        }
        return ArkUI_EnterKeyType::ARKUI_ENTER_KEY_TYPE_DONE;
    }

    static const std::unordered_map<std::string, ArkUI_FontWeight> FONT_WEIGHT_MAP = {
        {"bold", ARKUI_FONT_WEIGHT_BOLD},
        {"normal", ARKUI_FONT_WEIGHT_NORMAL},
        {"bolder", ARKUI_FONT_WEIGHT_BOLDER},
        {"lighter", ARKUI_FONT_WEIGHT_LIGHTER},
        {"medium", ARKUI_FONT_WEIGHT_MEDIUM},
        {"regular", ARKUI_FONT_WEIGHT_REGULAR}};

    static const std::unordered_map<std::string, ArkUI_FontStyle> FONT_STYLE_MAP = {
        {"normal", ARKUI_FONT_STYLE_NORMAL},
        {"italic", ARKUI_FONT_STYLE_ITALIC}};

    PlaceholderStyle TaroInput::GetPlaceholderStyle(const std::string& placeholderStyle) {
        PlaceholderStyle style;
        std::vector<std::string> params = TaroHelper::StringUtils::split(placeholderStyle, ";");
        for (auto& param : params) {
            std::vector<std::string> stylePair = TaroHelper::StringUtils::split(param, ":");
            if (stylePair.size() < 2) {
                continue;
            }
            boost::algorithm::trim(stylePair[0]);
            boost::algorithm::trim(stylePair[1]);

            if (stylePair[0] == "color") {
                std::optional<uint32_t> color = TaroHelper::StringUtils::parseColor(stylePair[1]);
                if (color.has_value()) {
                    style.color.set(color.value());
                }
            }
            if (stylePair[0] == "font-size") {
                style.fontSize.set(std::stof(stylePair[1]));
            }
            if (stylePair[0] == "font-style") {
                auto iter = FONT_STYLE_MAP.find(stylePair[1]);
                if (iter != FONT_STYLE_MAP.end()) {
                    style.fontStyle.set(iter->second);
                }
            }
            if (stylePair[0] == "font-weight") {
                auto iter = FONT_WEIGHT_MAP.find(stylePair[1]);
                if (iter != FONT_WEIGHT_MAP.end()) {
                    style.fontWeight.set(iter->second);
                }
            }
        }

        return style;
    }

    void TaroInput::Build(std::shared_ptr<TaroElement>& reuse_element) {
        std::shared_ptr<TaroTextInputNode> new_node = std::dynamic_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            new_node = std::make_shared<TaroTextInputNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            SetRenderNode(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->event_emitter_->clearNodeEvent(ark_handle);
            reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
            return;
        }
        if (ark_handle == nullptr) {
            auto parent = new_node->parent_ref_.lock();
            if (parent) {
                new_node->Build();
                parent->UpdateChild(new_node);
                updateListenEvent();
                new_node->ClearDifferOldStyleFromElement();
            }
            return;
        }
        new_node->SetArkUINodeHandle(ark_handle);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        reuse_element->event_emitter_->clearNodeEvent(ark_handle);
        updateListenEvent();
        reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
