//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
        void SetAttributesToRenderNode() override ;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
