//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
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
