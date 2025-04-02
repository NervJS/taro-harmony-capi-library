/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include "runtime/dom/element/view.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroSwiperItem : public TaroView {
        public:
        TaroSwiperItem(napi_value node);
        ~TaroSwiperItem() = default;

        void Build() override;
        void SetAttributesToRenderNode() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
