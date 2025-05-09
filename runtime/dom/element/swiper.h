/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_SWIPER_H
#define TARO_CAPI_HARMONY_DEMO_SWIPER_H

#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroSwiperAttributes : public CommonAttributes {
        // Swiper attributes
        TaroHelper::Optional<int32_t> index;
        TaroHelper::Optional<int32_t> usedIndex;
        TaroHelper::Optional<bool> loop;
        TaroHelper::Optional<float> duration; // Unit: ms, default value: 400
        TaroHelper::Optional<float> interval;
        TaroHelper::Optional<bool> vertical;
        TaroHelper::Optional<bool> autoPlay;
        TaroHelper::Optional<bool> indicator;
        TaroHelper::Optional<uint32_t> indicatorColor;
        TaroHelper::Optional<uint32_t> indicatorActiveColor;
        TaroHelper::Optional<Dimension> nextMargin;
        TaroHelper::Optional<Dimension> prevMargin;
        TaroHelper::Optional<uint32_t> displayCount;
        TaroHelper::Optional<bool> disableProgrammaticAnimation;
        TaroHelper::Optional<bool> disableTouch;
        TaroHelper::Optional<std::string> adjustHeight;

        TaroSwiperAttributes() {
            loop = false;
            duration = 500;
            interval = 5000;
            vertical = false;
            autoPlay = false;
            indicator = false;
            disableTouch = false;
            std::optional<uint32_t> color;
            color = TaroHelper::StringUtils::parseColor("rgba(0, 0, 0, .3)");
            if (color.has_value()) {
                indicatorColor = TaroHelper::Optional<uint32_t>(color.value());
            }
            color = TaroHelper::StringUtils::parseColor("#000000");
            if (color.has_value()) {
                indicatorActiveColor = TaroHelper::Optional<uint32_t>(color.value());
            }
        }
    };

    class TaroSwiper : public TaroElement {
        public:
        TaroSwiper(napi_value node);

        ~TaroSwiper();

        void Build() override;

        void SetAttributesToRenderNode() override;

        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        bool bindListenEvent(const std::string& event_name) override;
        void handleEvent();

        uint32_t GetDisplayCount();
        Dimension GetPrevMargin();
        Dimension GetNextMargin();
        bool IsAutoHeight();
        bool GetVertical();

        std::shared_ptr<TaroStackNode> autoHeightContainer_;
        std::shared_ptr<TaroStackNode> autoHeightWrap_;

        private:
        void updateIndex(const napi_value& value);
        void updateLoop(const napi_value& value);
        void updateDuration(const napi_value& value);
        void updateInterval(const napi_value& value);
        void updateVertical(const napi_value& value);
        void updateAutoPlay(const napi_value& value);
        void updateIndicator(const napi_value& value);
        void updateIndicatorOrActiveColor(const napi_value& value, std::string type);
        void updateNextMargin(const napi_value& value);
        void updatePrevMargin(const napi_value& value);
        void updateDisplayCount(const napi_value& value);

        void updateDisplayTouch(const napi_value& value);
        void updateDisableProgrammaticAnimation(const napi_value& value);
        void updateAdjustHeight(const napi_value& value);
        void onTransition(std::shared_ptr<TaroEvent::TaroEventBase> value);
        void SetWrapperHeight(std::shared_ptr<TaroSwiper> swiperElement, int move_index, float move_val);
        bool isIndicator_;
        bool is_vertical_ = false;
        std::unique_ptr<TaroSwiperAttributes> attributes_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_SWIPER_H
