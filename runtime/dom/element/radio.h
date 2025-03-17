//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef HARMONY_LIBRARY_RADIO_ELEMENT_H
#define HARMONY_LIBRARY_RADIO_ELEMENT_H

#include <arkui/native_node.h>

#include "runtime/dom/ark_nodes/radio.h"
#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroRadioAttributes : public TaroFormAttributes {
        TaroHelper::Optional<bool> checked;
        TaroHelper::Optional<uint32_t> color;
    };
    class TaroRadio : public FormWidget {
        public:
        TaroRadio(napi_value node);
        ~TaroRadio() = default;
        void SetAttributesToRenderNode() override;
        void Build() override;

        virtual TaroRadioAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        void GetColorAttribute(const napi_value& napiValue);
        void GetCheckedAttribute(const napi_value& napiValue);
        void AddToGroupManager(const napi_value& napiValue);
        void SetColorAttribute();
        void SetValueAttribute() override;
        void SetCheckedAttribute();
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void callJSFunc(std::string eventType, std::shared_ptr<TaroNode> node = nullptr);
        void handleEvent();
        void SetGroupInfo();
        void ChangeSelectState();
        void SetParent(const std::shared_ptr<TaroNode>& node);
        void ChangeIDAttribute(const std::string& preValue, const std::string& curValue);
        void HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) override;
        bool bindListenEvent(const std::string& event_name) override;

        private:
        std::unique_ptr<TaroRadioAttributes> attributes_;
        std::shared_ptr<TaroRadioNode> radio_;
        std::shared_ptr<TaroNode> parent_ = nullptr;
        bool has_name_ = false;
        Dimension default_height_;
        Dimension default_mini_height_;
        Dimension default_width_;
        Dimension default_mini_width_;
        float textWidth_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_RADIO_ELEMENT_H
