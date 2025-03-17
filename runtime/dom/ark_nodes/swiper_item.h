//
// Created on 2024/6/7.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
#pragma once

#include "arkui_node.h"
#include "runtime/dom/ark_nodes/stack.h"

namespace TaroRuntime::TaroDOM {
class TaroSwiperItemNode : public TaroStackNode {
    public:
    TaroSwiperItemNode(const TaroElementRef element);

    ~TaroSwiperItemNode() override;

    void Build() override;

    void SetStyle(StylesheetRef style_ref) override;
    
    void Measure() override;
};

} // namespace TaroRuntime::TaroDOM
