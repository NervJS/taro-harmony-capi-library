//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_VIEW_H
#define TARO_CAPI_HARMONY_DEMO_VIEW_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroView : public TaroElement {
        public:
        TaroView(napi_value node);

        virtual ~TaroView() = default;

        void Build() override;
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;
        void GetNodeAttributes() override;
        void SetAttributesToRenderNode() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        protected:
        bool bindListenEvent(const std::string&) override;

        private:
        std::vector<std::weak_ptr<TaroElement>> catch_container_list_;
        void handleOnTouch();
        void CheckTextAlign();
        void BuildScrollView();
        void SetOverflowScrollAttribute();
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_VIEW_H
