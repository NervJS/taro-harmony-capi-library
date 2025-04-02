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

            SetRenderNode(render_swiper);
        }
    }

    bool TaroSwiper::bindListenEvent(const std::string& event_name) {
        if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SWIPER_ON_CHANGE, event_name);
        } else if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "transition") {
            bool is_vertical = is_vertical_;
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SWIPER_ON_TRANSITION, event_name);
        } else {
            return false;
        }
        return true;
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
                std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetIndex(attributes_->index.value(), attributes_->disableProgrammaticAnimation.value_or(false));
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
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetLoop(attributes_->loop.value());
        }
    }

    void TaroSwiper::updateDuration(const napi_value& value) {
        NapiGetter getter(value);
        auto duration = getter.Double();
        if (duration.has_value()) {
            attributes_->duration.set(duration.value());
        }
        if (is_init_ && attributes_->duration.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetDuration(attributes_->duration.value());
        }
    }

    void TaroSwiper::updateInterval(const napi_value& value) {
        NapiGetter getter(value);
        auto interval = getter.Double();
        if (interval.has_value()) {
            attributes_->interval.set(interval.value());
        }
        if (is_init_ && attributes_->interval.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetInterval(attributes_->interval.value());
        }
    }

    void TaroSwiper::updateVertical(const napi_value& value) {
        NapiGetter getter(value);
        auto vertical = getter.BoolNull();
        if (vertical.has_value()) {
            attributes_->vertical.set(vertical.value());
        }
        if (is_init_ && attributes_->vertical.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetVertical(attributes_->vertical.value());
        }
    }

    void TaroSwiper::updateAutoPlay(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->autoPlay.set(autoPlay.value());
        }
        if (is_init_ && attributes_->autoPlay.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetAutoPlay(attributes_->autoPlay.value());
        }
    }

    void TaroSwiper::updateDisableProgrammaticAnimation(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->disableProgrammaticAnimation.set(autoPlay.value());
        }
    }

    void TaroSwiper::updateDisplayTouch(const napi_value& value) {
        NapiGetter getter(value);
        auto autoPlay = getter.BoolNull();
        if (autoPlay.has_value()) {
            attributes_->disableTouch.set(autoPlay.value());
        }
        if (is_init_ && attributes_->disableTouch.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetDisableSwiper(attributes_->disableTouch.value());
        }
    }

    void TaroSwiper::updateIndicator(const napi_value& value) {
        NapiGetter getter(value);
        auto indicator = getter.BoolNull();
        if (indicator.has_value()) {
            attributes_->indicator.set(indicator.value());
        }
        isIndicator_ = attributes_->indicator.value(); // we have default value
        auto render_swiper = std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode());
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
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetIndicatorActiveColor(ArkUI_SwiperIndicatorType::ARKUI_SWIPER_INDICATOR_TYPE_DOT, attributes_->indicatorColor.has_value(), attributes_->indicatorActiveColor.value());
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
            double margin = attributes_->nextMargin.value().ParseToVp(GetHeadRenderNode()->GetDimensionContext()).value_or(0.0f);
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetNextMargin(margin);
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
            double margin = attributes_->prevMargin.value().ParseToVp(GetHeadRenderNode()->GetDimensionContext()).value_or(0.0f);
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetPrevMargin(margin);
        }
    }

    void TaroSwiper::updateDisplayCount(const napi_value& value) {
        NapiGetter getter(value);
        auto displayCount = getter.Int32();
        if (displayCount.has_value()) {
            attributes_->displayCount.set(displayCount.value());
        }
        if (is_init_ && attributes_->displayCount.has_value()) {
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode())->SetDisplayCount(attributes_->displayCount.value());
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
            default:
                break;
        }
    }

    void TaroSwiper::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        std::shared_ptr<TaroSwiperNode> render_swiper =
            std::static_pointer_cast<TaroSwiperNode>(GetHeadRenderNode());

        DimensionContextRef dimContext = render_swiper->GetDimensionContext();
        if (attributes_->index.has_value()) {
            if (attributes_->usedIndex != attributes_->index) {
                attributes_->usedIndex.set(attributes_->index);
                render_swiper->SetIndex(attributes_->index.value(), attributes_->disableProgrammaticAnimation.value_or(false));
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

        render_swiper->SetStyle(style_);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
