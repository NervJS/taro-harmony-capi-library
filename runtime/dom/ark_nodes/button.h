/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroButtonNode : public TaroRenderNode {
        public:
        TaroButtonNode(const TaroElementRef element);

        ~TaroButtonNode();

        void SetStyle(StylesheetRef style_ref) override;

        void Build() override;

        protected:
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> textNodeStyle_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
