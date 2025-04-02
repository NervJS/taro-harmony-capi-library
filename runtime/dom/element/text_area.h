/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_TEXT_AREA_H
#define TARO_CAPI_HARMONY_DEMO_TEXT_AREA_H

#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"
#include "runtime/dom/element/input.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroTextAreaAttributes : public TaroInputAttributes {
        TaroHelper::Optional<bool> autoHeight;
    };

    class TaroTextArea : public TaroInput {
        public:
        TaroTextArea(napi_value node);

        virtual ~TaroTextArea() = default;

        void Build() override;

        virtual TaroTextAreaAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        std::unique_ptr<TaroTextAreaAttributes> attributes_;

        void GetNodeAttributes() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void SetAttributesToRenderNode() override;

        void GetAutoHeightAttribute(const napi_value& value);
        void SetAutoHeightAttribute();

        protected:
        int GetInputType(const std::string& type) override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_TEXT_AREA_H
