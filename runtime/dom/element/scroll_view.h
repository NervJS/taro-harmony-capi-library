/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_SCROLLVIEW_H
#define TARO_CAPI_HARMONY_SCROLLVIEW_H

#include "runtime/dom/element/element.h"
#include "runtime/dom/element/scroller_container.h"

namespace TaroRuntime {
namespace TaroDOM {

    class TaroScrollView : public TaroScrollerContainer {
        public:
        TaroScrollView(napi_value node);
        ~TaroScrollView() = default;

        void Build() override;
    
        protected:
        void SetScrollXAttribute() override;
        void SetScrollYAttribute() override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_SCROLLVIEW_H
