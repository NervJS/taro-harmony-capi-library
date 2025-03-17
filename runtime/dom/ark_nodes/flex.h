//
// Created on 2024/7/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
