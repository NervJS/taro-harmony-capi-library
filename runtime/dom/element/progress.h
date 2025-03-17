//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_PROGRESS_H
#define HARMONY_LIBRARY_PROGRESS_H

#include <cstdint>

#include "runtime/dom/ark_nodes/progress.h"
#include "runtime/dom/ark_nodes/text_span.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    struct TaroProgressAttributes : public CommonAttributes {
        TaroHelper::Optional<int32_t> percent;
        TaroHelper::Optional<bool> showInfo;
        TaroHelper::Optional<uint32_t> borderRadius;
        TaroHelper::Optional<uint32_t> fontSize;
        TaroHelper::Optional<uint32_t> strokeWidth;
        TaroHelper::Optional<uint32_t> activeColor;
        TaroHelper::Optional<uint32_t> backgroundColor;
        TaroHelper::Optional<bool> active;
        TaroHelper::Optional<std::string> activeMode;
    };

    class TaroProgress : public TaroElement {
        public:
        TaroProgress(napi_value node);
        ~TaroProgress();

        void Build() override;

        virtual TaroProgressAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        std::unique_ptr<TaroProgressAttributes> attributes_;

        void GetNodeAttributes() override;
        void SetAttributesToRenderNode() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void GetPercentAttribute(const napi_value& value);
        void GetShowInfoAttribute(const napi_value& value);
        void GetBorderRadiusAttribute(const napi_value& value);
        void GetFontSizeAttribute(const napi_value& value);
        void GetStrokeWidthAttribute(const napi_value& value);
        void GetActiveColorAttribute(const napi_value& value);
        void GetBackgroundColorAttribute(const napi_value& value);
        void GetActiveAttribute(const napi_value& value);
        void GetActiveModeAttribute(const napi_value& value);

        void SetPercentAttribute();
        void SetShowInfoAttribute();
        void SetBorderRadiusAttribute();
        void SetFontSizeAttribute();
        void SetStrokeWidthAttribute();
        void SetActiveColorAttribute();
        void SetBackgroundColorAttribute();
        void SetActiveAttribute();
        void SetActiveModeAttribute();

        private:
        std::shared_ptr<TaroProgressNode> progress_;
        std::shared_ptr<TaroTextSpanNode> span_;
    };
} // namespace TaroDOM

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_PROGRESS_H
