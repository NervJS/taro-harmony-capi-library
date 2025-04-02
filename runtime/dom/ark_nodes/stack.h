/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroStackNode : public TaroRenderNode {
        public:
        TaroStackNode(const TaroElementRef element);

        ~TaroStackNode() override;

        void SetStyle(StylesheetRef style_ref) override;

        void Build() override;
        //     void insertChild(TaroRenderNode& child, std::size_t index);
        //     void removeChild(TaroRenderNode& child);
        void SetTranslate(float x, float y, float z);
        void SetScale(float x, float y);
        void SetAlign(const ArkUI_Alignment& align);

        protected:
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> textNodeStyle_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime
