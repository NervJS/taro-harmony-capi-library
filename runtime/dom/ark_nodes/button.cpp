//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "button.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroButtonNode::TaroButtonNode(const TaroElementRef element)
        : TaroRenderNode(element) {
    }

    TaroButtonNode::~TaroButtonNode() {}

    void TaroButtonNode::SetStyle(StylesheetRef style_ref) {
        TaroRenderNode::SetStyle(style_ref);
        textNodeStyle_ = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        // 父元素文本相关样式修改
        if (CheckAndSetFontStyle(textNodeStyle_)) {
            auto element = element_ref_.lock();
            if (element) {
                for (auto child : element->child_nodes_) {
                    auto childEle = std::dynamic_pointer_cast<TaroElement>(child);
                    if (childEle->isTextElement()) {
                        auto renderNode = childEle->GetHeadRenderNode();
                        if (renderNode) {
                            renderNode->SetStyle(TaroCSSOM::TaroStylesheet::Stylesheet::assign(textNodeStyle_, renderNode->element_ref_.lock()->style_));
                            renderNode->SetContent();
                        }
                    }
                }
            }
        }
    }

    void TaroButtonNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_STACK));
    }
} // namespace TaroDOM
} // namespace TaroRuntime
