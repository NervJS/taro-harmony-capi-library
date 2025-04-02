/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "text_input.h"

#include <arkui/native_type.h>

#include "./differ/differ_macro.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"

namespace TaroRuntime {
namespace TaroDOM {
    void TaroTextInputNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_TEXT_INPUT));
    }

    void TaroTextInputNode::Paint() {
        // TODO 暂没支持decoration
        TaroRenderNode::Paint();
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(fontSize_)) {
            if (IS_PAINT_DIFFER_PROPERTY_REMOVED(fontSize_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderRadius(GetArkUINodeHandle());
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setFontSize(GetArkUINodeHandle(), paintDiffer_.paint_style_->fontSize_.value);
            }
        }
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(fontWeight_)) {
            if (IS_PAINT_DIFFER_PROPERTY_REMOVED(fontWeight_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setFontWeight(GetArkUINodeHandle());
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setFontWeight(GetArkUINodeHandle(), paintDiffer_.paint_style_->fontWeight_.value);
            }
        }
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(color_)) {
            if (IS_PAINT_DIFFER_PROPERTY_REMOVED(color_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setColor(GetArkUINodeHandle());
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setColor(GetArkUINodeHandle(), paintDiffer_.paint_style_->color_.value);
            }
        }
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(textAlign_)) {
            if (IS_PAINT_DIFFER_PROPERTY_REMOVED(textAlign_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTextAlign(GetArkUINodeHandle());
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTextAlign(GetArkUINodeHandle(), paintDiffer_.paint_style_->textAlign_.value);
            }
        }
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(lineHeight_)) {
            if (IS_PAINT_DIFFER_PROPERTY_REMOVED(lineHeight_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setLineHeight(GetArkUINodeHandle());
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setLineHeight(GetArkUINodeHandle(), paintDiffer_.paint_style_->lineHeight_.value);
            }
        }
    }

    void TaroTextInputNode::SetInputType(ArkUI_TextInputType keyboardType) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = keyboardType};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_TYPE, &item);
    }

    void TaroTextInputNode::SetTextContent(const std::string& value) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = value.c_str()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_TEXT, &item);
    }

    void TaroTextInputNode::SetConfirmType(ArkUI_EnterKeyType confirmType) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = confirmType};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_ENTER_KEY_TYPE, &item);
    }

    void TaroTextInputNode::SetMaxLength(int32_t maxLength) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = maxLength};
        if (maxLength < 0) {
            // Note: 小于 0 不限制
            nativeNodeApi->resetAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_MAX_LENGTH);
        } else {
            ArkUI_AttributeItem item = {&value, 1};
            nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_MAX_LENGTH, &item);
        }
    }

    void TaroTextInputNode::SetAutoFocus(bool autoFocus) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = static_cast<int32_t>(autoFocus)};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_FOCUS_STATUS, &item);
    }

    void TaroTextInputNode::SetEnableKeyboardOnFocus(bool enabled) {
        ArkUI_NumberValue value = {.i32 = static_cast<int>(enabled)};
        ArkUI_AttributeItem item = {&value, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &item);
    }

    void TaroTextInputNode::SetPlaceholder(const std::string& placeholder) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = placeholder.c_str()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_PLACEHOLDER, &item);
    }

    void TaroTextInputNode::SetPlaceholderStyle(const PlaceholderStyle& placeholderStyle) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[3];
        std::string fontFamily = "HarmonyOS Sans";
        ArkUI_AttributeItem item = {value, 3, .string = fontFamily.c_str()};

        if (placeholderStyle.fontSize.has_value()) {
            value[0].f32 = placeholderStyle.fontSize.value();
        } else {
            value[0].f32 = 16;
        }
        if (placeholderStyle.fontStyle.has_value()) {
            value[1].i32 = placeholderStyle.fontStyle.value();
        } else {
            value[1].i32 = ARKUI_FONT_STYLE_NORMAL;
        }
        if (placeholderStyle.fontWeight.has_value()) {
            value[2].i32 = placeholderStyle.fontWeight.value();
        } else {
            value[2].i32 = ARKUI_FONT_WEIGHT_NORMAL;
        }
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_PLACEHOLDER_FONT, &item);

        if (placeholderStyle.color.has_value()) {
            SetPlaceholderTextColor(placeholderStyle.color.value());
        }
    }

    void TaroTextInputNode::SetPlaceholderTextColor(uint32_t color) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.u32 = color};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_PLACEHOLDER_COLOR, &item);
    }

    void TaroTextInputNode::SetCustomKeyBoard(bool enable) {
        ArkUI_NumberValue value = {.i32 = static_cast<int>(enable)};
        ArkUI_AttributeItem item = {&value, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_CUSTOM_KEYBOARD, &item);
    }

    void TaroTextInputNode::SetEditing(bool status) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = static_cast<int>(status)};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_INPUT_EDITING, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
