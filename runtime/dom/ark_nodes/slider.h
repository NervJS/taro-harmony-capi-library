/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_NODE_SLIDER_H
#define TARO_CAPI_HARMONY_DEMO_NODE_SLIDER_H

#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroSliderNode : public TaroRenderNode {
        public:
        TaroSliderNode(const TaroElementRef element);
        ~TaroSliderNode();

        void Build() override;

        void SetStep(int32_t step);
        void SetValue(int32_t value);
        void SetMinValue(int32_t minValue);
        void SetMaxValue(int32_t maxValue);
        void SetActiveColor(uint32_t color);
        void SetBackgroundColor(uint32_t color);
        void SetBlockColor(uint32_t color);
        void SetBlockSize(int32_t size);
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_NODE_SLIDER_H
