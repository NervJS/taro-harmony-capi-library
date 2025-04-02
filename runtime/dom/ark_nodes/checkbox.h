/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_CHECKBOX_H
#define HARMONY_LIBRARY_CHECKBOX_H
#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"
namespace TaroRuntime {
namespace TaroDOM {
    class TaroCheckboxNode : public TaroRenderNode {
        public:
        TaroCheckboxNode(const TaroElementRef element);
        ~TaroCheckboxNode();
        void Build() override;
        void SetChecked(bool checked);
        void SetColor(uint32_t color);
        void SetShape(std::string shape);
        void ChangeChecked();
        void SetZeroMargin();
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_CHECKBOX_H
