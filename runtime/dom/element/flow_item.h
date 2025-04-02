/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_FLOW_ITEM_H
#define HARMONY_LIBRARY_FLOW_ITEM_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    class TaroFlowItem : public TaroElement {
        public:
        TaroFlowItem(napi_value node);
        ~TaroFlowItem();

        void Build() override;
        void SetAttributesToRenderNode() override;
        void Build(std::shared_ptr<TaroElement>& reuse_element) override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_FLOW_ITEM_H
