/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_MOVABLE_AREA_H
#define HARMONY_LIBRARY_MOVABLE_AREA_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    class TaroMovableArea : public TaroElement {
        public:
        TaroMovableArea(napi_value node);

        ~TaroMovableArea() = default;

        void SetAttributesToRenderNode() override;

        void Build() override;
        void Build(std::shared_ptr<TaroElement>& reuse_element) override;

        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        private:
        bool scale_area = false;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_MOVABLE_AREA_H
