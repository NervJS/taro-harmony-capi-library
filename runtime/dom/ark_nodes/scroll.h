/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
