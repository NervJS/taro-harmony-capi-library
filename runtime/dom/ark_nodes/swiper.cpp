
//
// Created on 2024/6/14.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "swiper.h"

#include <arkui/native_type.h>

#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/element/swiper.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {

namespace TaroDOM {

    TaroSwiperNode::TaroSwiperNode(const TaroElementRef element)
        : TaroRenderNode(element) {
        // 中断测量
        TaroSetCanMeasureChild(ygNodeRef, false);
    }

    TaroSwiperNode::~TaroSwiperNode() {}

    void TaroSwiperNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_SWIPER));
        ArkUI_NumberValue sizeValue[] = {};
        ArkUI_AttributeItem sizeItem = {sizeValue, 1};
        sizeValue[0].i32 = ArkUI_Alignment::ARKUI_ALIGNMENT_TOP_START;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_ALIGNMENT, &sizeItem);
    }

    void TaroSwiperNode::SetStyle(StylesheetRef style_ref) {
        if (style_ref) {
            if (!style_ref->width.has_value()) {
                style_ref->width.set(Dimension{1, DimensionUnit::PERCENT});
            }
            if (!style_ref->height.has_value()) {
                style_ref->height.set({1, DimensionUnit::PERCENT});
            }
            if (!style_ref->overflow.has_value()) {
                style_ref->overflow.set(PropertyType::Overflow::Hidden);
            }
        }
        TaroRenderNode::SetStyle(style_ref);
    }

    void TaroSwiperNode::SetOverflowNow(bool isHidden) {
        SetOverflow(isHidden ? PropertyType::Overflow::Hidden : PropertyType::Overflow::Visible);
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setOverflow(GetArkUINodeHandle(), isHidden ? PropertyType::Overflow::Hidden : PropertyType::Overflow::Visible);
    }

    void TaroSwiperNode::Layout() {
        TaroRenderNode::Layout();
        LayoutSelf();
    }

    void TaroSwiperNode::LayoutSelf() {
        TaroSetCanMeasureChild(ygNodeRef, true);

        for (auto child : children_refs_) {
            child->Measure();
            child->LayoutAll();
        }

        TaroSetCanMeasureChild(ygNodeRef, false);
    }

    ChildAvailableSize TaroSwiperNode::GetChildAvailableSize() {
        // 单独根据自身约束条件测量子节点
        if (auto element = element_ref_.lock()) {
            auto swiper = std::static_pointer_cast<TaroSwiper>(element);
            // step 1 获取约束宽
            float availableWidth = layoutDiffer_.computed_style_.width;
            float availableHeight = layoutDiffer_.computed_style_.height;
            // step 2 获取列数
            uint32_t count = swiper->GetDisplayCount();
            // step 3 获取边距
            Dimension prevMargin = swiper->GetPrevMargin();
            Dimension nextMargin = swiper->GetNextMargin();
            DimensionContextRef context = GetDimensionContext();
            // step 4 计算内部可用宽度
            if (swiper->GetVertical()) {
                availableHeight = availableHeight - prevMargin.ConvertToVp(context) - nextMargin.ConvertToVp(context);
                availableHeight = std::max(availableHeight, 0.0f);
                availableHeight = availableHeight / count;
            } else {
                availableWidth = availableWidth - prevMargin.ConvertToVp(context) - nextMargin.ConvertToVp(context);
                availableWidth = std::max(availableWidth, 0.0f);
                availableWidth = availableWidth / count;
            }

            return {availableWidth, availableHeight};
        }

        return ChildAvailableSize({});
    };

    void TaroSwiperNode::SetIndex(int32_t index, bool disableAnimation) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
    
        const ArkUI_AttributeItem *_item = nativeNodeApi->getAttribute(GetArkUINodeHandle(), NODE_SWIPER_INDEX);
        auto currentIndex = _item->value[0].i32;
        if(currentIndex == index) return;
     
        ArkUI_NumberValue arkUI_NumberValue[2] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
        arkUI_NumberValue[0].i32 = index;
        arkUI_NumberValue[1].i32 = !disableAnimation;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_SWIPE_TO_INDEX, &item);
    }

    void TaroSwiperNode::SetDisableSwiper(bool disableSwiper) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        arkUI_NumberValue[0].i32 = disableSwiper;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_DISABLE_SWIPE, &item);
    }

    void TaroSwiperNode::SetLoop(bool loop) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        arkUI_NumberValue[0].i32 = loop;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_LOOP, &item);
    }

    void TaroSwiperNode::SetDuration(float duration) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        arkUI_NumberValue[0].f32 = duration;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_DURATION, &item);
    }

    void TaroSwiperNode::SetInterval(float interval) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        arkUI_NumberValue[0].f32 = interval;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_INTERVAL, &item);
    }

    void TaroSwiperNode::SetVertical(bool isVertical) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        int32_t vertical = isVertical ? 1 : 0;
        arkUI_NumberValue[0].i32 = vertical;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_VERTICAL, &item);
    }

    void TaroSwiperNode::SetAutoPlay(bool autoPlay) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        int32_t autoPlayValue = autoPlay ? 1 : 0;
        arkUI_NumberValue[0].i32 = autoPlayValue;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_AUTO_PLAY, &item);
    }

    void TaroSwiperNode::SetIndicator(bool showIndicator) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue arkUI_NumberValue[] = {};
        ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
        int32_t showIndicatorValue = showIndicator ? 1 : 0;
        arkUI_NumberValue[0].i32 = showIndicatorValue;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_SHOW_INDICATOR, &item);
    }

    void TaroSwiperNode::SetIndicatorActiveColor(
        ArkUI_SwiperIndicatorType type, uint32_t indicatorColor,
        uint32_t indicatorActiveColor) {
        ArkUI_SwiperIndicator *indicator =
            OH_ArkUI_SwiperIndicator_Create(type);
        if (indicator == nullptr) {
            TARO_LOG_ERROR("TaroSwiperNode", "Create swiper indicator failed");
            return;
        }
        OH_ArkUI_SwiperIndicator_SetColor(indicator, indicatorColor);
        OH_ArkUI_SwiperIndicator_SetSelectedColor(indicator,
                                                  indicatorActiveColor);
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[] = {};
        value[0].i32 = type;
        ArkUI_AttributeItem item = {value, 1, nullptr, indicator};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SWIPER_INDICATOR, &item);
        OH_ArkUI_SwiperIndicator_Dispose(indicator);
    }

    void TaroSwiperNode::SetNextMargin(float nextMargin) {
        auto weakSelf = std::weak_ptr<TaroSwiperNode>(std::static_pointer_cast<TaroSwiperNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, nextMargin]() {
            if (auto self_ = weakSelf.lock()) {
                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue arkUI_NumberValue[] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].f32 = nextMargin;
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_SWIPER_NEXT_MARGIN, &item);
                self_->LayoutSelf();
            }
        });
    }

    void TaroSwiperNode::SetPrevMargin(float prevMargin) {
        auto weakSelf = std::weak_ptr<TaroSwiperNode>(std::static_pointer_cast<TaroSwiperNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, prevMargin]() {
            if (auto self_ = weakSelf.lock()) {
                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue arkUI_NumberValue[] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].f32 = prevMargin;
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_SWIPER_PREV_MARGIN, &item);
                self_->LayoutSelf();
            }
        });
    }

    void TaroSwiperNode::SetDisplayCount(uint32_t displayCount) {
        auto weakSelf = std::weak_ptr<TaroSwiperNode>(std::static_pointer_cast<TaroSwiperNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, displayCount]() {
            if (auto self_ = weakSelf.lock()) {
                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue arkUI_NumberValue[] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].i32 = displayCount;
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_SWIPER_DISPLAY_COUNT, &item);
                self_->LayoutSelf();
            }
        });
    }

} // namespace TaroDOM
} // namespace TaroRuntime