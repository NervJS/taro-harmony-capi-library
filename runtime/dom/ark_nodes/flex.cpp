//
// Created on 2024/7/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "flex.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroFlexNode::TaroFlexNode(const TaroElementRef element)
        : TaroRenderNode(element) {}
    TaroFlexNode::~TaroFlexNode() {}
    void TaroFlexNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_FLEX));
    }
} // namespace TaroDOM
} // namespace TaroRuntime
