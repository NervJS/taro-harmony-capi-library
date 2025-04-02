/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./scroll.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroScrollNode::TaroScrollNode(TaroElementRef element)
        : TaroScrollContainerNode(element) {}

    TaroScrollNode::~TaroScrollNode() {}

    void TaroScrollNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_SCROLL));
        // align
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = ARKUI_ALIGNMENT_TOP_START;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_ALIGNMENT, &item);
        // clip
        value[0].i32 = 1;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_CLIP, &item);
        TaroScrollContainerNode::setEdgeEffect(ARKUI_EDGE_EFFECT_NONE);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
