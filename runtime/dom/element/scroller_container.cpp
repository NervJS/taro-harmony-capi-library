/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "scroller_container.h"

#include <arkui/native_node.h>

#include "helper/SwitchManager.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/ark_nodes/scroll_container.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_scroll.h"
#include "runtime/dom/event/event_hm/event_types/event_type.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroScrollerContainer::TaroScrollerContainer(napi_value node)
        : TaroElement(node),
          attributes_(std::make_unique<TaroScrollerContainerAttributes>()) {}

    bool TaroScrollerContainer::IsScrollX() {
        bool isScrollX = this->attributes_->scrollX.value_or(false);
        if (!isScrollX) {
            // 缺少 overflow
            bool hasFlexDirection = this->style_->flexDirection.has_value();
            if (hasFlexDirection) {
                ArkUI_FlexDirection flexDirection = this->style_->flexDirection.value();
                if (flexDirection == ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW) {
                    isScrollX = true;
                }
            }
        }

        return isScrollX;
    }

    void TaroScrollerContainer::Build() {
        if (!is_init_) {
            SetScrollLeftAttribute();
            SetScrollTopAttribute();
        }
    }

    OffsetResult TaroScrollerContainer::GetCurrentOffset() {
        return {scrollLeft_, scrollTop_};
    }

    void TaroScrollerContainer::ScrollTo(ScrollToParams params) {
        ScrollToParams scroll_params({scrollLeft_, scrollTop_, 300});

        if (std::isfinite(params.xOffset)) {
            scroll_params.xOffset = params.xOffset;
        }
        if (std::isfinite(params.yOffset)) {
            scroll_params.yOffset = params.yOffset;
        }
        if (std::isfinite(params.duration)) {
            scroll_params.duration = params.duration;
        }

        auto node = GetHeadRenderNode();
        if (node != nullptr) {
            std::shared_ptr<TaroScrollContainerNode> render_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
            render_view->SetScrollOffset(
                scroll_params.xOffset,
                scroll_params.yOffset,
                true,
                scroll_params.duration);
        }
    }

    void TaroScrollerContainer::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        switch (name) {
            case ATTRIBUTE_NAME::SCROLL_X:
                GetScrollXAttribute(value);
                if (is_init_) {
                    SetScrollXAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SCROLL_Y:
                GetScrollYAttribute(value);
                if (is_init_) {
                    SetScrollYAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SCROLL_TOP:
                GetScrollTopAttribute(value);
                if (is_init_) {
                    SetScrollTopAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SCROLL_LEFT:
                GetScrollLeftAttribute(value);
                if (is_init_) {
                    SetScrollLeftAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SHOW_SCROLL_BAR:
                GetShowScrollBarAttribute(value);
                if (is_init_) {
                    SetShowScrollBarAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SCROLL_WITH_ANIMATION:
                GetScrollWithAnimation(value);
                if (is_init_) {
                    SetScrollWithAnimation();
                }
                break;
            case ATTRIBUTE_NAME::SCROLL_ANIMATION_DURATION:
                GetScrollAnimationDuration(value);
                if (is_init_) {
                    SetScrollAnimationDuration();
                }
                break;
            default:
                break;
        }
    }

    void TaroScrollerContainer::GetScrollXAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> scrollX = getter.BoolNull();
        if (scrollX.has_value()) {
            attributes_->scrollX.set(scrollX.value());
        }
    }

    void TaroScrollerContainer::SetScrollDirection() {
        auto render_scroll_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
        bool unSet = !this->attributes_->scrollY.has_value() && !this->attributes_->scrollX.has_value();
        is_scroll_x = IsScrollX();
        is_scroll_y = this->attributes_->scrollY.value_or(false);
        if (is_scroll_x) {
            render_scroll_view->setScrollDirection(ARKUI_SCROLL_DIRECTION_HORIZONTAL);
            render_scroll_view->setScrollEnableScrollInteraction(true);
            if (!style_->height.has_value()) {
                render_scroll_view->SetHeight(Dimension{0, DimensionUnit::AUTO}, Dimension{0, DimensionUnit::NONE});
            }
        } else if (is_scroll_y || unSet) { // 如果两个方向都没有设，小程序文档是不滚，小程序实际行为是y轴滚，所以按实际的来
            render_scroll_view->setScrollDirection(ARKUI_SCROLL_DIRECTION_VERTICAL);
            render_scroll_view->setScrollEnableScrollInteraction(true);
            if (!style_->width.has_value()) {
                render_scroll_view->SetWidth(Dimension{0, DimensionUnit::AUTO});
            }
        } else {
            // 当x和y方向都不能滚动的时候，没有设置方向，而是阻止手势的响应，
            // 因为如果原先设置scrolly并且在y方向有一定滚动量的情况，把状态切成两个方向都不滚动
            // ark底层会把原来y轴的滚动变成x轴的滚动
            render_scroll_view->setScrollEnableScrollInteraction(false);
        }
    }

    void TaroScrollerContainer::GetScrollLeftAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<double> scrollLeft = getter.Double();
        if (scrollLeft.has_value()) {
            attributes_->scrollLeft.set(scrollLeft.value());
        }
    }

    void TaroScrollerContainer::SetScrollLeftAttribute() {
        if (attributes_->scrollLeft.has_value()) {
            std::shared_ptr<TaroScrollContainerNode> render_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
            render_view->SetScrollOffset(
                attributes_->scrollLeft.value(),
                scrollTop_,
                attributes_->scrollWithAnimation.has_value() ? attributes_->scrollWithAnimation.value() : false,
                attributes_->scrollAnimationDuration.has_value() ? attributes_->scrollAnimationDuration.value() : 500);
        }
    }

    void TaroScrollerContainer::GetScrollTopAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<double> scrollTop = getter.Double();
        if (scrollTop.has_value()) {
            attributes_->scrollTop.set(scrollTop.value());
        }
    }

    void TaroScrollerContainer::SetScrollTopAttribute() {
        if (attributes_->scrollTop.has_value()) {
            std::shared_ptr<TaroScrollContainerNode> render_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
            render_view->SetScrollOffset(
                scrollLeft_,
                attributes_->scrollTop.value(),
                attributes_->scrollWithAnimation.has_value() ? attributes_->scrollWithAnimation.value() : false,
                attributes_->scrollAnimationDuration.has_value() ? attributes_->scrollAnimationDuration.value() : 500);
        }
    }

    void TaroScrollerContainer::GetScrollYAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> scrollY = getter.BoolNull();
        if (scrollY.has_value()) {
            attributes_->scrollY.set(scrollY.value());
        }
    }

    void TaroScrollerContainer::SetScrollYAttribute() {
        SetScrollDirection();
    }

    void TaroScrollerContainer::SetScrollXAttribute() {
        SetScrollDirection();
    }

    void TaroScrollerContainer::GetShowScrollBarAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> showScrollbar = getter.BoolNull();
        if (showScrollbar.has_value()) {
            attributes_->showScrollbar.set(showScrollbar.value());
        }
    }

    void TaroScrollerContainer::SetShowScrollBarAttribute() {
        auto render_scroll_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
        if (attributes_->showScrollbar.has_value()) {
            render_scroll_view->setScrollBarDisplay(
                attributes_->showScrollbar.value()
                    ? ARKUI_SCROLL_BAR_DISPLAY_MODE_ON
                    : ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
        }
    }

    void TaroScrollerContainer::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        is_scroll_x = IsScrollX();
        auto render_scroll_view = std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode());
        SetScrollXAttribute();
        render_scroll_view->SetStyle(style_);
        render_scroll_view->SetOverflow(style_->overflow.value_or(overflowMode_));
        if (!style_->display.has_value()) {
            render_scroll_view->SetDisplay(PropertyType::Display::Flex);
            render_scroll_view->SetFlexDirection(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW);
        }

        if (is_scroll_x) {
            render_scroll_view->setNestedMode(ARKUI_SCROLL_NESTED_MODE_SELF_ONLY, ARKUI_SCROLL_NESTED_MODE_SELF_ONLY);
        } else if (parentType_ != TARO_ELEMENT_MAX) {
            render_scroll_view->setNestedMode(ARKUI_SCROLL_NESTED_MODE_PARENT_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
        }

        SetShowScrollBarAttribute();
        SetScrollAnimationDuration();
        SetScrollWithAnimation();
    }

    void TaroScrollerContainer::GetScrollWithAnimation(const napi_value& value) {
        NapiGetter getter(value);
        auto withAnimation = getter.BoolNull();
        if (withAnimation.has_value()) {
            attributes_->scrollWithAnimation.set(withAnimation.value());
        }
    }

    void TaroScrollerContainer::SetScrollWithAnimation() {
        if (attributes_->scrollWithAnimation.has_value()) {
            std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode())->SetScrollOffset(scrollLeft_, scrollTop_, attributes_->scrollWithAnimation.value(), attributes_->scrollAnimationDuration.has_value() ? attributes_->scrollAnimationDuration.value() : 500);
        }
    }

    void TaroScrollerContainer::GetScrollAnimationDuration(const napi_value& value) {
        NapiGetter getter(value);
        auto duration = getter.Int32();
        if (duration.has_value()) {
            attributes_->scrollAnimationDuration.set(duration.value());
        }
    }

    void TaroScrollerContainer::SetScrollAnimationDuration() {
        if (attributes_->scrollAnimationDuration.has_value()) {
            std::static_pointer_cast<TaroScrollContainerNode>(GetHeadRenderNode())->SetScrollOffset(scrollLeft_, scrollTop_, attributes_->scrollWithAnimation.has_value() ? attributes_->scrollWithAnimation.value() : false, attributes_->scrollAnimationDuration.value());
        }
    }

    bool TaroScrollerContainer::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "touchcancel") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_CANCEL, event_name);
        } else if (event_name == "scrollstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SCROLL_START_ON_SCROLL, event_name);
        } else if (event_name == "scrollend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SCROLL_END_ON_SCROLL, event_name);
        } else if (event_name == "scrolltolower") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_REACH_END_ON_SCROLL, event_name);
        } else if (event_name == "scroll") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_DID_SCROLL_ON_SCROLL, event_name,
                                          [&](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                                              const ArkUI_AttributeItem* item = NativeNodeApi::getInstance()->getAttribute(GetHeadRenderNode()->GetArkUINodeHandle(), NODE_SCROLL_OFFSET);
                                              // detail填充
                                              auto scrollEvent = std::static_pointer_cast<TaroEvent::TaroEventDidScroll>(event);
                                              NapiSetter::SetProperty(scrollEvent->detail(), "scrollTop", item->value[1].f32);
                                              NapiSetter::SetProperty(scrollEvent->detail(), "scrollLeft", item->value[0].f32);

                                              return 0;
                                          });
        } else {
            return false;
        }
        return true;
    }

    void TaroScrollerContainer::handleOnScroll() {
        auto scroll_fun = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            const ArkUI_AttributeItem* item = NativeNodeApi::getInstance()->getAttribute(GetHeadRenderNode()->GetArkUINodeHandle(), NODE_SCROLL_OFFSET);
            scrollLeft_ = item->value[0].f32;
            scrollTop_ = item->value[1].f32;
            return 0;
        };
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_DID_SCROLL_ON_SCROLL, "scrollRecord", scroll_fun);
    }

    void TaroScrollerContainer::findParentType() {
        auto parent = std::dynamic_pointer_cast<TaroElement>(GetParentNode());
        while (parent) {
            if (parent->tag_name_ == TAG_NAME::LIST_ITEM) {
                parentType_ = TARO_ELEMENT_LIST;
                break;
            }
            if (parent->tag_name_ == TAG_NAME::SCROLL_VIEW) {
                parentType_ = TARO_ELEMENT_SCROLL;
                break;
            }
            parent = std::dynamic_pointer_cast<TaroElement>(parent->GetParentNode());
        }
    }

    void TaroScrollerContainer::handleVisibleNode() {
        for (auto it = this->on_visible_node.begin(); it != this->on_visible_node.end();) {
            if (auto node = it->lock()) {
                auto visible_element = std::static_pointer_cast<TaroElement>(node);

                if (visible_element->is_init_ && !visible_element->info_.registerd) {
                    it = this->on_visible_node.erase(it);
                    continue;
                } else {
                    ++it;
                }

                visible_element->handleVisibilityInfo();
            } else {
                it = this->on_visible_node.erase(it);
            }
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
