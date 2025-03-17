//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef HARMONY_LIBRARY_CHECKBOX_ELEMENT_H
#define HARMONY_LIBRARY_CHECKBOX_ELEMENT_H
#include <arkui/native_node.h>

#include "runtime/dom/ark_nodes/checkbox.h"
#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroCheckboxAttributes : public TaroFormAttributes {
        TaroHelper::Optional<bool> checked;
        TaroHelper::Optional<std::string> color;
        TaroHelper::Optional<std::string> shape;
    };
    class TaroCheckbox : public FormWidget {
        public:
        TaroCheckbox(napi_value node);
        ~TaroCheckbox() = default;
        void SetAttributesToRenderNode() override;
        void Build() override;

        virtual TaroCheckboxAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        void handleEvent();
        void callJSFunc(std::string eventType, int32_t select);
        void GetColorAttribute(const napi_value& napiValue);
        void GetCheckedAttribute(const napi_value& napiValue);
        void GetShapeAttribute(const napi_value& napiValue);
        void SetColorAttribute();
        void SetCheckedAttribute();
        void SetShapeAttribute();
        void SetParent(const std::shared_ptr<TaroNode>& node);
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void SetLink();
        void ChangeSelectState();
        void ChangeIDAttribute(const std::string& preValue, const std::string& curValue);
        void HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) override;
        bool bindListenEvent(const std::string& event_name) override;

        std::unique_ptr<TaroCheckboxAttributes> attributes_;

        std::shared_ptr<TaroCheckboxNode> checkbox_;
        std::shared_ptr<TaroNode> parent_ = nullptr;
        bool link = false;
        Dimension default_height_;
        Dimension default_mini_height_;
        Dimension default_width_;
        Dimension default_mini_width_;
        float textWidth_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_CHECKBOX_ELEMENT_H
