/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "text_area.h"

#include <arkui/native_type.h>

#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroTextAreaNode::TaroTextAreaNode(const TaroElementRef element)
        : TaroTextInputNode(element) {
    }

    TaroTextAreaNode::~TaroTextAreaNode() {}

    void TaroTextAreaNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_TEXT_AREA));
    }

    void TaroTextAreaNode::SetAutoHeight(bool autoHeight) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[1] = {};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = autoHeight;
        // TODO no ArkUI_NodeAttributeType
        // nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_MAX_LENGTH, &item);
    }

    void TaroTextAreaNode::SetInputType(ArkUI_TextInputType keyboardType) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = keyboardType};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_TYPE, &item);
    }

    void TaroTextAreaNode::SetTextContent(const std::string& value) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = value.c_str()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_TEXT, &item);
    }

    void TaroTextAreaNode::SetConfirmType(ArkUI_EnterKeyType confirmType) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = confirmType};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_ENTER_KEY_TYPE, &item);
    }

    void TaroTextAreaNode::SetMaxLength(int32_t maxLength) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = maxLength};
        if (maxLength < 0) {
            // Note: 小于 0 不限制
            nativeNodeApi->resetAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_MAX_LENGTH);
        } else {
            ArkUI_AttributeItem item = {&value, 1};
            nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_MAX_LENGTH, &item);
        }
    }

    void TaroTextAreaNode::SetEnableKeyboardOnFocus(bool enabled) {
        ArkUI_NumberValue value = {.i32 = static_cast<int>(enabled)};
        ArkUI_AttributeItem item = {&value, 1};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_ENABLE_KEYBOARD_ON_FOCUS, &item);
    }

    void TaroTextAreaNode::SetPlaceholder(const std::string& placeholder) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = placeholder.c_str()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_PLACEHOLDER, &item);
    }

    void TaroTextAreaNode::SetPlaceholderStyle(const PlaceholderStyle& placeholderStyle) {
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
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_PLACEHOLDER_FONT, &item);

        if (placeholderStyle.color.has_value()) {
            SetPlaceholderTextColor(placeholderStyle.color.value());
        }
    }

    void TaroTextAreaNode::SetPlaceholderTextColor(uint32_t color) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.u32 = color};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_PLACEHOLDER_COLOR, &item);
    }

    void TaroTextAreaNode::SetEditing(bool status) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.i32 = static_cast<int>(status)};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TEXT_AREA_EDITING, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
