/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "swiper.h"

#include <arkui/native_node.h>

#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NapiSetter.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "runtime/dom/element/swiper_item.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_swiper.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroSwiper::TaroSwiper(napi_value node)
        : TaroElement(node),
          isIndicator_(false),
          attributes_(std::make_unique<TaroSwiperAttributes>()) {}

    TaroSwiper::~TaroSwiper() {}

    void TaroSwiper::Build() {
        if (!is_init_) {
            // create render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());

            auto render_swiper = std::make_shared<TaroSwiperNode>(element);
            render_swiper->Build();
            render_swiper->SetShouldPosition(false);

            if (IsAutoHeight()) {
                autoHeightContainer_ = std::make_shared<TaroStackNode>(element);
                autoHeightContainer_->Build();
                autoHeightWrap_ = std::make_shared<TaroStackNode>(element);
                autoHeightWrap_->Build();
                autoHeightWrap_->SetAlign(ARKUI_ALIGNMENT_START);

                autoHeightContainer_->AppendChild(autoHeightWrap_);
                autoHeightWrap_->AppendChild(render_swiper);
                SetHeadRenderNode(autoHeightContainer_);
            } else {
                SetHeadRenderNode(render_swiper);
            }

            SetFooterRenderNode(render_swiper);
        }
    }
    void TaroSwiper::handleEvent() {
        auto onTransition = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            this->onTransition(event);
            return 0;
        };
        event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SWIPER_ON_CHANGE, "change", nullptr, GetFooterRenderNode()->GetArkUINodeHandle());
        if (IsAutoHeight()) {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SWIPER_ON_TRANSITION, "transition", onTransition, GetFooterRenderNode()->GetArkUINodeHandle());
        }
    }

    bool TaroSwiper::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "transition") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SWIPER_ON_TRANSITION, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroSwiper::SetWrapperHeight(std::shared_ptr<TaroSwiper> swiperElement, int move_index, float move_val) {
        auto& child_nodes = swiperElement->child_nodes_;
        auto leftItem = std::dynamic_pointer_cast<TaroSwiperItem>(child_nodes[(move_index - 1 + child_nodes.size()) % child_nodes.size()]);
        auto rightItem = std::dynamic_pointer_cast<TaroSwiperItem>(child_nodes[move_index]);
        if (leftItem && rightItem) {
            auto curH = leftItem->GetFooterRenderNode()->layoutDiffer_.computed_style_.height;
            auto nextH = rightItem->GetFooterRenderNode()->layoutDiffer_.computed_style_.height;
            // 改变swiper容器的高度
            autoHeightWrap_->SetLayoutFlag(LAYOUT_STATE_FLAG::IS_IGNORE_SIZE);
            autoHeightWrap_->SetHeight((nextH - curH) * (1 - move_val) + curH);
        }
    }

    void TaroSwiper::onTransition(std::shared_ptr<TaroEvent::TaroEventBase> event) {
        if (IsAutoHeight()) {
            auto swiperEvent = std::dynamic_pointer_cast<TaroEvent::TaroEventTransitionInSwiper>(event);
            if (swiperEvent) {
                auto swiperElement = std::static_pointer_cast<TaroSwiper>(swiperEvent->cur_target_);
                if (swiperEvent->move_val_ >= 0) {
                    SetWrapperHeight(swiperElement, swiperEvent->current_index_, swiperEvent->move_val_);
                } else if (swiperEvent->move_val_ <= 0) {
                    auto nextIndex = swiperEvent->current_index_ + 1;
                    auto& child_nodes = swiperElement->child_nodes_;
                    SetWrapperHeight(swiperElement, (nextIndex + child_nodes.size()) % child_nodes.size(), 1 + swiperEvent->move_val_);
                }
            }
        }
    }

    bool TaroSwiper::IsAutoHeight() {
        return attributes_->adjustHeight.value_or("") == "current" && !is_vertical_;
    }

    void TaroSwiper::updateIndex(const napi_value& value) {
        NapiGetter getter(value);
        auto index = getter.Int32();
        if (index.has_value()) {
            attributes_->index.set(index.value());
        }
        if (is_init_ && attributes_->index.has_value()) {
            if (attributes_->usedIndex != attributes_->index) {
                attributes_->usedIndex.set(attributes_->index);
                DirtyTaskPipeline::GetInstance()->RegistryNextTick([node = weak_from_this()]() {
                    if (auto that = std::static_pointer_cast<TaroSwiper>(node.lock())) {
                        std::static_pointer_cast<TaroSwiperNode>(that->GetFooterRenderNode())->SetIndex(that->attributes_->index.value(), that->attributes_->disableProgrammaticAnimation.value_or(false));
                    }
                });
            }
        }
    }

    void TaroSwiper::updateLoop(const napi_value& value) {
        NapiGetter getter(value);
        auto loop = getter.BoolNull();
        if (loop.has_value()) {
            attributes_->loop.set(loop.value());
        }
        if (is_init_ && attributes_->loop.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetLoop(attributes_->loop.value());
        }
    }

    void TaroSwiper::updateDuration(const napi_value& value) {
        NapiGetter getter(value);
        auto duration = getter.Double();
        if (duration.has_value()) {
            attributes_->duration.set(duration.value());
        }
        if (is_init_ && attributes_->duration.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetDuration(attributes_->duration.value());
        }
    }

    void TaroSwiper::updateInterval(const napi_value& value) {
        NapiGetter getter(value);
        auto interval = getter.Double();
        if (interval.has_value()) {
            attributes_->interval.set(interval.value());
        }
        if (is_init_ && attributes_->interval.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetInterval(attributes_->interval.value());
        }
    }

    void TaroSwiper::updateVertical(const napi_value& value) {
        NapiGetter getter(value);
        auto vertical = getter.BoolNull();
        if (vertical.has_value()) {
            attributes_->vertical.set(vertical.value());
        }
        if (is_init_ && attributes_->vertical.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetVertical(attributes_->vertical.value());
        }
    }

    void TaroSwiper::updateAutoPlay(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->autoPlay.set(autoPlay.value());
        }
        if (is_init_ && attributes_->autoPlay.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetAutoPlay(attributes_->autoPlay.value());
        }
    }

    void TaroSwiper::updateDisableProgrammaticAnimation(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->disableProgrammaticAnimation.set(autoPlay.value());
        }
    }

    void TaroSwiper::updateAdjustHeight(const napi_value& value) {
        NapiGetter getter(value);
        auto adjustHeight = getter.String();
        if (adjustHeight.has_value()) {
            attributes_->adjustHeight.set(adjustHeight.value());
        } else {
            attributes_->adjustHeight.reset();
        }
    }

    void TaroSwiper::updateDisplayTouch(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->disableTouch.set(autoPlay.value());
        }
        if (is_init_ && attributes_->disableTouch.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetDisableSwiper(attributes_->disableTouch.value());
        }
    }

    void TaroSwiper::updateIndicator(const napi_value& value) {
        NapiGetter getter(value);
        auto indicator = getter.BoolNull();
        if (indicator.has_value()) {
            attributes_->indicator.set(indicator.value());
        }
        isIndicator_ = attributes_->indicator.value(); // we have default value
        auto render_swiper = std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode());
        if (is_init_) {
            render_swiper->SetIndicator(isIndicator_);
        }
        if (is_init_ && isIndicator_) {
            render_swiper->SetIndicatorActiveColor(
                ArkUI_SwiperIndicatorType::ARKUI_SWIPER_INDICATOR_TYPE_DOT,
                attributes_->indicatorColor.value(),
                attributes_->indicatorActiveColor.value());
        }
    }

    void TaroSwiper::updateIndicatorOrActiveColor(const napi_value& value, std::string type) {
        NapiGetter getter(value);
        auto indicatorColor = getter.String();
        std::optional<uint32_t> color;
        if (indicatorColor.has_value()) {
            color = TaroHelper::StringUtils::parseColor(indicatorColor.value());
        }
        if (color.has_value()) {
            if (type == "indicatorColor") {
                attributes_->indicatorColor.set(color.value());
            } else {
                attributes_->indicatorActiveColor.set(color.value());
            }
        }
        if (is_init_ && isIndicator_) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetIndicatorActiveColor(ArkUI_SwiperIndicatorType::ARKUI_SWIPER_INDICATOR_TYPE_DOT, attributes_->indicatorColor.has_value(), attributes_->indicatorActiveColor.value());
        }
    }

    void TaroSwiper::updateNextMargin(const napi_value& value) {
        NapiGetter getter(value);
        auto type = getter.Type();
        if (type == napi_string) {
            auto nextMargin = getter.String();
            if (nextMargin.has_value()) {
                attributes_->nextMargin.set(Dimension::FromString(nextMargin.value()));
            }
        } else if (type == napi_number) {
            auto nextMargin = getter.Double();
            if (nextMargin.has_value()) {
                attributes_->nextMargin.set(nextMargin.value());
            }
        }
        if (is_init_ && attributes_->nextMargin.has_value()) {
            double margin = attributes_->nextMargin.value().ParseToVp(GetFooterRenderNode()->GetDimensionContext()).value_or(0.0f);
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetNextMargin(margin);
        }
    }

    void TaroSwiper::updatePrevMargin(const napi_value& value) {
        NapiGetter getter(value);
        auto type = getter.Type();
        if (type == napi_string) {
            auto prevMargin = getter.String();
            if (prevMargin.has_value()) {
                attributes_->prevMargin.set(Dimension::FromString(prevMargin.value()));
            }
        } else if (type == napi_number) {
            auto prevMargin = getter.Double();
            if (prevMargin.has_value()) {
                attributes_->prevMargin.set(prevMargin.value());
            }
        }

        if (is_init_ && attributes_->prevMargin.has_value()) {
            double margin = attributes_->prevMargin.value().ParseToVp(GetFooterRenderNode()->GetDimensionContext()).value_or(0.0f);
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetPrevMargin(margin);
        }
    }

    void TaroSwiper::updateDisplayCount(const napi_value& value) {
        NapiGetter getter(value);
        auto displayCount = getter.Int32();
        if (displayCount.has_value()) {
            attributes_->displayCount.set(displayCount.value());
        }
        if (is_init_ && attributes_->displayCount.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode())->SetDisplayCount(attributes_->displayCount.value());
        }
    }

    uint32_t TaroSwiper::GetDisplayCount() {
        return attributes_->displayCount.has_value() ? attributes_->displayCount.value() : 1;
    }

    Dimension TaroSwiper::GetPrevMargin() {
        return attributes_->prevMargin.has_value() ? attributes_->prevMargin.value() : 0;
    }
    Dimension TaroSwiper::GetNextMargin() {
        return attributes_->nextMargin.has_value() ? attributes_->nextMargin.value() : 0;
    }

    bool TaroSwiper::GetVertical() {
        return attributes_->vertical.has_value() ? attributes_->vertical.value() : false;
    }

    void TaroSwiper::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::INDEX:
                updateIndex(value);
                break;
            case ATTRIBUTE_NAME::LOOP:
                updateLoop(value);
                break;
            case ATTRIBUTE_NAME::DURATION:
                updateDuration(value);
                break;
            case ATTRIBUTE_NAME::INTERVAL:
                updateInterval(value);
                break;
            case ATTRIBUTE_NAME::VERTICAL:
                updateVertical(value);
                break;
            case ATTRIBUTE_NAME::AUTOPLAY:
                updateAutoPlay(value);
                break;
            case ATTRIBUTE_NAME::INDICATOR:
                updateIndicator(value);
                break;
            case ATTRIBUTE_NAME::INDICATOR_COLOR:
                updateIndicatorOrActiveColor(value, "indicatorColor");
                break;
            case ATTRIBUTE_NAME::INDICATOR_ACTIVE_COLOR:
                updateIndicatorOrActiveColor(value, "indicatorActiveColor");
                break;
            case ATTRIBUTE_NAME::NEXT_MARGIN:
                updateNextMargin(value);
                break;
            case ATTRIBUTE_NAME::PREV_MARGIN:
                updatePrevMargin(value);
                break;
            case ATTRIBUTE_NAME::DISPLAY_COUNT:
                updateDisplayCount(value);
                break;
            case ATTRIBUTE_NAME::DISABLE_TOUCH:
                updateDisplayTouch(value);
                break;
            case ATTRIBUTE_NAME::DISABLE_PROGRAMMATIC_ANIMATION:
                updateDisableProgrammaticAnimation(value);
                break;
            case ATTRIBUTE_NAME::ADJUST_HEIGHT:
                updateAdjustHeight(value);
            default:
                break;
        }
    }

    void TaroSwiper::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        std::shared_ptr<TaroSwiperNode> render_swiper = std::static_pointer_cast<TaroSwiperNode>(GetFooterRenderNode());

        DimensionContextRef dimContext = render_swiper->GetDimensionContext();
        if (attributes_->index.has_value()) {
            if (attributes_->usedIndex != attributes_->index) {
                attributes_->usedIndex.set(attributes_->index);
                DirtyTaskPipeline::GetInstance()->RegistryNextTick([node = weak_from_this(), render_swiper]() {
                    if (auto that = std::static_pointer_cast<TaroSwiper>(node.lock())) {
                        render_swiper->SetIndex(that->attributes_->index.value(), that->attributes_->disableProgrammaticAnimation.value_or(false));
                    }
                });
            }
        }
        if (attributes_->disableTouch.has_value()) {
            render_swiper->SetDisableSwiper(attributes_->disableTouch.value());
        }
        if (attributes_->loop.has_value()) {
            render_swiper->SetLoop(attributes_->loop.value());
        }
        if (attributes_->duration.has_value()) {
            render_swiper->SetDuration(attributes_->duration.value());
        }
        if (attributes_->interval.has_value()) {
            render_swiper->SetInterval(attributes_->interval.value());
        }
        if (attributes_->vertical.has_value()) {
            is_vertical_ = attributes_->vertical.value();
            render_swiper->SetVertical(attributes_->vertical.value());
        }
        if (attributes_->autoPlay.has_value()) {
            render_swiper->SetAutoPlay(attributes_->autoPlay.value());
        }
        if (attributes_->indicator.has_value()) {
            render_swiper->SetIndicator(attributes_->indicator.value());
        }
        if (isIndicator_) {
            render_swiper->SetIndicatorActiveColor(
                ArkUI_SwiperIndicatorType::ARKUI_SWIPER_INDICATOR_TYPE_DOT,
                attributes_->indicatorColor.value(),
                attributes_->indicatorActiveColor.value());
        }
        if (attributes_->nextMargin.has_value()) {
            if (auto nextMarginValue = attributes_->nextMargin.value().ParseToVp(dimContext); nextMarginValue.has_value()) {
                render_swiper->SetNextMargin(nextMarginValue.value());
            }
        }
        if (attributes_->prevMargin.has_value()) {
            if (auto prevMarginValue = attributes_->prevMargin.value().ParseToVp(dimContext); prevMarginValue.has_value()) {
                render_swiper->SetPrevMargin(prevMarginValue.value());
            }
        }
        if (attributes_->displayCount.has_value()) {
            render_swiper->SetDisplayCount(attributes_->displayCount.value());
        }

        if (IsAutoHeight()) {
            autoHeightContainer_->SetStyle(style_);
            if (!style_->overflow.has_value()) {
                GetHeadRenderNode()->SetOverflow(PropertyType::Overflow::Hidden);
            }
            auto emptyStyle = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
            autoHeightWrap_->SetStyle(emptyStyle);
            render_swiper->SetStyle(emptyStyle);
        } else {
            render_swiper->SetStyle(style_);
        }

        handleEvent();
    }
} // namespace TaroDOM
} // namespace TaroRuntime
