/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_LIST_ITEM_GROUP_H
#define TARO_HARMONY_CPP_LIST_ITEM_GROUP_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    class TaroListItemGroup : public TaroElement {
        public:
        TaroListItemGroup(napi_value node);
        ~TaroListItemGroup();

        void Build() override;
        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void setStickyHeader(std::shared_ptr<TaroRenderNode> renderNode);

        private:
        void GetSpace(const napi_value& value);
        void SetSpace();

        TaroHelper::Optional<double> space;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_LIST_ITEM_GROUP_H
