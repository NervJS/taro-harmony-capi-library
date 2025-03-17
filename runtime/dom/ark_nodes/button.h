//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
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
