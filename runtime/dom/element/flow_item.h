//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_FLOW_ITEM_H
