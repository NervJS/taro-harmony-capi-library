/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "flex.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroFlexNode::TaroFlexNode(const TaroElementRef element)
        : TaroRenderNode(element) {}
    TaroFlexNode::~TaroFlexNode() {}
    void TaroFlexNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_FLEX));
    }
} // namespace TaroDOM
} // namespace TaroRuntime
