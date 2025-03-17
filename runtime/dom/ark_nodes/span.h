//
// Created by zhutianjian on 24-6-7.
//

#ifndef TARO_HARMONY_SPAN_H
#define TARO_HARMONY_SPAN_H

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroSpanNode : public TaroRenderNode {
        public:
        TaroSpanNode(const TaroElementRef element);
        ~TaroSpanNode();

        void SetContent(std::string value);

        void Build() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_SPAN_H
