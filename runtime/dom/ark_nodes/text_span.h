/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_SPAN_H
#define TARO_HARMONY_SPAN_H

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    // Note: 区别于 Text 节点，这是 Element 内部使用的文本组件，仅包含基础实现
    class TaroTextSpanNode : public TaroRenderNode {
        public:
        TaroTextSpanNode(const TaroElementRef element);
        ~TaroTextSpanNode();

        void SetContent(std::string value);

        void Build() override;

        ArkUI_NodeHandle GetTextArkNode();

        private:
        ArkUI_NodeHandle m_InnerTextNode = nullptr;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_SPAN_H
