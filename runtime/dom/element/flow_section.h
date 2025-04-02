/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_FLOW_SECTION_H
#define TARO_HARMONY_CPP_FLOW_SECTION_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct SectionMargin {
        double marginTop = 0.0;
        double marginRight = 0.0;
        double marginBottom = 0.0;
        double marginLeft = 0.0;
    };

    class TaroFlowSection : public TaroElement {
        public:
        TaroFlowSection(napi_value node);
        ~TaroFlowSection();

        void Build() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;
        void onAppendChild(std::shared_ptr<TaroNode> child) override;
        void onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) override;
        void onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) override;
        int32_t column_;
        TaroHelper::Optional<double> rowGap_;
        TaroHelper::Optional<double> columnGap_;
        TaroHelper::Optional<SectionMargin> margin_;

        private:
        // 从Napi获取值，设置到属性上
        void GetColumnAttribute(const napi_value& value);
        void GetRowGapAttribute(const napi_value& value);
        void GetColumnGapAttribute(const napi_value& value);
        void GetMarginAttribute(const napi_value& value);
        double GetDoubleAttributeValue(const napi_value& value);

        void SetMarginAttribute();
        void SetColumnAttribute();
        void SetRowGapAttribute();
        void SetColumnGapAttribute();
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_FLOW_SECTION_H
