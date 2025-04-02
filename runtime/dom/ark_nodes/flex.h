/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_FLEX_H
#define HARMONY_LIBRARY_FLEX_H

#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroFlexNode : public TaroRenderNode {
        public:
        TaroFlexNode(const TaroElementRef element);
        ~TaroFlexNode();

        void Build() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_FLEX_H
