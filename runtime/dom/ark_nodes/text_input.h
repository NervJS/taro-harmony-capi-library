/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_TEXT_INPUT_H
#define TARO_CAPI_HARMONY_DEMO_TEXT_INPUT_H

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct PlaceholderStyle {
        TaroHelper::Optional<uint32_t> color;
        TaroHelper::Optional<float> fontSize;
        TaroHelper::Optional<ArkUI_FontStyle> fontStyle;
        TaroHelper::Optional<ArkUI_FontWeight> fontWeight;
    };

    class TaroTextInputNode : public TaroRenderNode {
        public:
        TaroTextInputNode(const TaroElementRef element)
            : TaroRenderNode(element) {
            SetShouldPosition(false);
        }

        ~TaroTextInputNode() = default;

        void Build() override;

        void Paint() override;

        void SetInputType(ArkUI_TextInputType type);

        void SetTextContent(const std::string& value);

        void SetConfirmType(ArkUI_EnterKeyType confirmType);

        void SetMaxLength(int32_t maxLength);

        void SetAutoFocus(bool autoFocus);

        void SetEnableKeyboardOnFocus(bool enabled);

        void SetPlaceholder(const std::string& placeholder);

        void SetPlaceholderStyle(const PlaceholderStyle& placeholderStyle);

        void SetPlaceholderTextColor(uint32_t color);

        void SetCustomKeyBoard(bool enable);

        void SetEditing(bool status);
    };
} // namespace TaroDOM

} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_TEXT_INPUT_H
