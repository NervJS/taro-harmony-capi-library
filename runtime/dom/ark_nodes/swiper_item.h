/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
