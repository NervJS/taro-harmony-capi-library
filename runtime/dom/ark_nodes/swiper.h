//
// Created on 2024/6/14.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#pragma once

#include "runtime/dom/ark_nodes/arkui_node.h"
#include "yoga/YGValue.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct ChildAvailableSize {
        float width = YGUndefined;
        float height = YGUndefined;
    };

    class TaroSwiperNode : public TaroRenderNode {
        public:
        TaroSwiperNode(const TaroElementRef element);

        ~TaroSwiperNode();

        void Build() override;

        void SetStyle(StylesheetRef style_ref) override;
        void Layout() override;
        ChildAvailableSize GetChildAvailableSize ();

        // set swiper properties
        void SetIndex(int32_t index, bool disableAnimation = false);
    
        void SetDisableSwiper(bool loop);

        void SetLoop(bool loop);

        void SetDuration(float duration);

        void SetInterval(float interval);

        void SetVertical(bool vertical);

        void SetAutoPlay(bool autoPlay);

        void SetIndicator(bool indicator);

        void SetIndicatorActiveColor(ArkUI_SwiperIndicatorType type,
                                     uint32_t indicatorColor,
                                     uint32_t indicatorActiveColor);
    
        void SetOverflowNow(bool isHidden);

        void SetNextMargin(float nextMargin);

        void SetPrevMargin(float prevMargin);

        void SetDisplayCount(uint32_t displayCount);

        private:
        void LayoutSelf();
    };

} // namespace TaroDOM
} // namespace TaroRuntime