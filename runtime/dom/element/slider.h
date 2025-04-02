/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_SLIDER_H
#define TARO_CAPI_HARMONY_DEMO_SLIDER_H

#include "runtime/dom/ark_nodes/slider.h"
#include "runtime/dom/ark_nodes/text_span.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroSliderAttributes : public TaroFormAttributes {
        TaroHelper::Optional<bool> showValue;
        TaroHelper::Optional<int32_t> step;
        TaroHelper::Optional<int32_t> value;
        TaroHelper::Optional<int32_t> minValue;
        TaroHelper::Optional<int32_t> maxValue;
        TaroHelper::Optional<int32_t> blockSize;
        TaroHelper::Optional<std::string> activeColor;
        TaroHelper::Optional<std::string> backgroundColor;
        TaroHelper::Optional<std::string> blockColor;
    };

    class TaroSlider : public FormWidget {
        public:
        TaroSlider(napi_value node);

        ~TaroSlider() = default;

        void GetNodeAttributes() override;
        virtual void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void SetAttributesToRenderNode() override;

        void Build() override;

        virtual TaroSliderAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        std::unique_ptr<TaroSliderAttributes> attributes_;

        bool bindListenEvent(const std::string& event_name) override;
        virtual void bindSelfEventHandle() override;

        void GetStepAttribute(const napi_value& value);
        void GetValueAttribute(const napi_value& value) override;
        void GetMinValueAttribute(const napi_value& value);
        void GetMaxValueAttribute(const napi_value& value);
        void GetShowValueAttribute(const napi_value& value);
        void GetBlockSizeAttribute(const napi_value& value);
        void GetActiveColorAttribute(const napi_value& value);
        void GetBackgroundColorAttribute(const napi_value& value);
        void GetBlockColorAttribute(const napi_value& value);

        void SetStepAttribute();
        void SetValueAttribute() override;
        void SetMinValueAttribute();
        void SetMaxValueAttribute();
        void SetShowValueAttribute();
        void SetBlockSizeAttribute();
        void SetActiveColorAttribute();
        void SetBackgroundColorAttribute();
        void SetBlockColorAttribute();

        private:
        std::shared_ptr<TaroSliderNode> slider_;
        std::shared_ptr<TaroTextSpanNode> span_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_SLIDER_H
