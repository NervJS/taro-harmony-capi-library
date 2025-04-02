/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_LIST_ITEM_H
#define HARMONY_LIBRARY_LIST_ITEM_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    class TaroListItem : public TaroElement {
        public:
        TaroListItem(napi_value node);
        ~TaroListItem();

        void Build() override;
        void Build(std::shared_ptr<TaroElement>& reuse_element) override;
        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_LIST_ITEM_H
