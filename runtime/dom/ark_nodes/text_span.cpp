/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "text_span.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroTextSpanNode::TaroTextSpanNode(TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroTextSpanNode::~TaroTextSpanNode() {}

    void TaroTextSpanNode::SetContent(std::string value) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = value.c_str()};
        nativeNodeApi->setAttribute(m_InnerTextNode, NODE_TEXT_CONTENT, &item);
    }

    void TaroTextSpanNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_STACK));
        m_InnerTextNode = nativeNodeApi->createNode(ARKUI_NODE_TEXT);
        nativeNodeApi->addChild(GetArkUINodeHandle(), m_InnerTextNode);
    }

    ArkUI_NodeHandle TaroTextSpanNode::GetTextArkNode() {
        return m_InnerTextNode;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
