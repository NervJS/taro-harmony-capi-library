/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_INPUT_H
#define TARO_CAPI_HARMONY_DEMO_INPUT_H

#include <arkui/native_node.h>

#include "runtime/dom/ark_nodes/text_input.h"
#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {

    struct TaroInputAttributes : public TaroFormAttributes {
        // input attributes
        TaroHelper::Optional<ArkUI_TextInputType> type;
        TaroHelper::Optional<ArkUI_EnterKeyType> confirmType;
        TaroHelper::Optional<int32_t> maxLength;
        TaroHelper::Optional<bool> focus;
        TaroHelper::Optional<std::string> placeholder;
        TaroHelper::Optional<PlaceholderStyle> placeholderStyle;
        TaroHelper::Optional<uint32_t> placeholderTextColor;
        TaroHelper::Optional<bool> adjustPosition = true;
        TaroHelper::Optional<bool> editing;
    };

    class TaroInput : public FormWidget {
        public:
        TaroInput(napi_value node);

        virtual ~TaroInput() = default;

        void Build() override;
        void Build(std::shared_ptr<TaroElement>& reuse_element) override;

        virtual TaroInputAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        std::unique_ptr<TaroInputAttributes> attributes_;

        virtual void GetNodeAttributes() override;
        virtual void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        virtual void SetAttributesToRenderNode() override;

        bool bindListenEvent(const std::string& event_name) override;
        virtual void bindSelfEventHandle() override;

        // 从Napi获取值，设置到属性上
        void GetPasswordAttribute(const napi_value& value);
        void GetTypeAttribute(const napi_value& value);
        void GetConfirmTypeAttribute(const napi_value& value);
        void GetMaxLengthTypeAttribute(const napi_value& value);
        void GetFocusTypeAttribute(const napi_value& value);
        void GetPlaceholderAttribute(const napi_value& value);
        void GetPlaceholderStyleAttribute(const napi_value& value);
        void GetPlaceholderTextColorAttribute(const napi_value& value);
        void GetAdjustPosition(const napi_value& value);
        void GetEditingAttribute(const napi_value& value);
        // 将属性设置到RenderNode上
        void SetValueAttribute() override;
        void SetPasswordAttribute();
        void SetTypeAttribute();
        void SetConfirmTypeAttribute();
        void SetMaxLengthTypeAttribute();
        void SetFocusTypeAttribute();
        void SetPlaceholderAttribute();
        void SetPlaceholderStyleAttribute();
        void SetPlaceholderTextColorAttribute();
        void SetAdjustPosition();
        void SetEditingAttribute();
        void SetEditingAttribute(bool isEditing);

        std::string pre_value = "";

        protected:
        virtual int GetInputType(const std::string& type);

        ArkUI_EnterKeyType GetConfirmType(const std::string& confirmType);

        PlaceholderStyle GetPlaceholderStyle(const std::string& placeholderStyle);

        bool isPassword{false};

        bool is_appeared = false;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_INPUT_H
