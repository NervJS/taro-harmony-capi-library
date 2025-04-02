/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "span.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroSpanNode::TaroSpanNode(TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroSpanNode::~TaroSpanNode() {}

    void TaroSpanNode::SetContent(std::string value) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = value.c_str()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SPAN_CONTENT, &item);
    }

    void TaroSpanNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_SPAN));
    }
} // namespace TaroDOM
} // namespace TaroRuntime
