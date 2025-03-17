//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TESTNDK_PAGE_CONTAINER_H
#define TESTNDK_PAGE_CONTAINER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroPageContainer : public TaroElement {
        public:
        // TODO: 临时性标记，当前页面 Remove 文本节点是否需要触发布局
        bool unstable_isTextNeedLayout = true;
    
        TaroPageContainer(napi_value node);

        ~TaroPageContainer();

        void Build() override;
        void SetAttributesToRenderNode() override;

        std::shared_ptr<TaroRenderNode> &getFixedRoot() {
            return fixed_root_;
        };

        void NotifyFixedElementVisibility() override;

        protected:
        std::shared_ptr<TaroRenderNode> fixed_root_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_PAGE_CONTAINER_H
