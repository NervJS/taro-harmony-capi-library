//
// Created on 2024/6/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TESTNDK_ARK_SCROLL_VIEW_H
#define TESTNDK_ARK_SCROLL_VIEW_H

#include "arkui_node.h"
#include "scroll_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroScrollNode : public TaroScrollContainerNode {
        public:
        TaroScrollNode(const TaroElementRef element);

        ~TaroScrollNode();

        void Build() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_ARK_SCROLL_VIEW_H
