//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
#ifndef TARO_CAPI_HARMONY_DEMO_AREA_H
#define TARO_CAPI_HARMONY_DEMO_AREA_H

#include "arkui_node.h"
#include "runtime/dom/ark_nodes/text_input.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroTextAreaNode : public TaroTextInputNode {
        public:
        TaroTextAreaNode(const TaroElementRef element);

        ~TaroTextAreaNode();

        void Build() override;

        void SetAutoHeight(bool autoHeight);

        void SetInputType(ArkUI_TextInputType type);

        void SetTextContent(const std::string& value);

        void SetConfirmType(ArkUI_EnterKeyType confirmType);

        void SetMaxLength(int32_t maxLength);

        void SetEnableKeyboardOnFocus(bool enabled);

        void SetPlaceholder(const std::string& placeholder);

        void SetPlaceholderStyle(const PlaceholderStyle& placeholderStyle);

        void SetPlaceholderTextColor(uint32_t color);

        void SetEditing(bool status);
    };
} // namespace TaroDOM

} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_AREA_H
