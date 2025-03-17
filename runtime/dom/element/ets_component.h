//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_ETS_H
#define TARO_CAPI_HARMONY_DEMO_ETS_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroEtsComponent : public TaroElement {
        public:
        TaroEtsComponent(napi_value node);

        ~TaroEtsComponent() = default;

        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void Build() override;
        void PostBuild() override;

        protected:
        bool bindListenEvent(const std::string&) override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_ETS_H
