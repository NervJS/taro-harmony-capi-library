/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroProgressNode : public TaroRenderNode {
        public:
        TaroProgressNode(const TaroElementRef element);

        ~TaroProgressNode();

        void Build() override;

        void SetPercent(float percent);

        void SetType(ArkUI_ProgressType type);

        void SetStrokeWidth(uint32_t value);

        void SetStrokeRadius(uint32_t value);

        void SetActiveColor(uint32_t color);

        void SetBackgroundColor(uint32_t color);
    };
} // namespace TaroDOM
} // namespace TaroRuntime
