//
// Created on 2024/6/22.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

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
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;

        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        private:
        bool scale_area = false;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_MOVABLE_AREA_H
