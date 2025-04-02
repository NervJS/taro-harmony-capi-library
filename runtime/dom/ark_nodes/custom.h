/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CUSTOM_H
#define TARO_HARMONY_CUSTOM_H

#include <arkui/native_node.h>

#include "arkui_node.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroCustomNode : public TaroRenderNode {
        public:
        TaroCustomNode(const TaroElementRef element);

        ~TaroCustomNode();

        virtual void OnMeasure(ArkUI_NodeCustomEvent *event) override;

        void Build() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CUSTOM_H
