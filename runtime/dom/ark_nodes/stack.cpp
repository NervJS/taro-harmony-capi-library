//
// Created on 2024/6/8.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./stack.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {

namespace TaroDOM {

    TaroStackNode::TaroStackNode(const TaroElementRef element)
        : TaroRenderNode(element) {
    }

    TaroStackNode::~TaroStackNode() {}

    void TaroStackNode::SetStyle(StylesheetRef style_ref) {
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
                            auto textStyle = renderNode->element_ref_.lock()->style_;
                            if (childEle->isPureTextNode()) {
                                textStyle = element->GetTextNodeStyleFromElement(element);
                            }
                            renderNode->SetStyle(textStyle);
                            renderNode->SetContent();
                        }
                    }
                }
            }
        }
    }

    void TaroStackNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_STACK));
        //         SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_CUSTOM));
        // SetCustomLayout();
    }

    void TaroStackNode::SetTranslate(float x, float y, float z) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[3] = {0};
        ArkUI_AttributeItem item = {value, 3};
        value[0].f32 = x;
        value[1].f32 = y;
        value[2].f32 = z;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_TRANSLATE, &item);
    }

    void TaroStackNode::SetScale(float x, float y) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[2] = {0};
        ArkUI_AttributeItem item = {value, 2};
        value[0].f32 = x;
        value[1].f32 = y;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SCALE, &item);
    }

    void TaroStackNode::SetAlign(const ArkUI_Alignment &align) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = align;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_ALIGNMENT, &item);
    }

} // namespace TaroDOM
} // namespace TaroRuntime
