/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "water_flow.h"

#include "flow_section.h"
#include "runtime/NapiSetter.h"
#include "runtime/dom/ark_nodes/water_flow.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_hm/event_types/event_scroll.h"
#include "runtime/dom/event/event_user/user_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroWaterFlow::TaroWaterFlow(napi_value node)
        : TaroScrollerContainer(node),
          attributes_(std::make_unique<TaroWaterFlowAttributes>()) {
        lazy_container = true;
    }

    TaroWaterFlow::~TaroWaterFlow() {}

    void TaroWaterFlow::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto water_flow = std::make_shared<TaroWaterFlowNode>(element);
            water_flow->Build();
            water_flow->SetShouldPosition(false);
            SetRenderNode(water_flow);
            for (int i = 0; i < child_nodes_.size(); ++i) {
                auto child = std::dynamic_pointer_cast<TaroDOM::TaroFlowSection>(child_nodes_[i]);
                if (child) {
                    child->BuildProcess();
                }
            }
            lazy_node = true;
            water_flow->initSections(child_nodes_);
            handleOnScrollIndex();
            findParentType();
            TaroScrollerContainer::Build();
        }
    }

    void TaroWaterFlow::GetLowerThresholdCount(const napi_value& value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->lowerThresholdCount.set(attribute.value());
        }
    }

    void TaroWaterFlow::GetUpperThresholdCount(const napi_value& value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->upperThresholdCount.set(attribute.value());
        }
    }

    void TaroWaterFlow::GetCacheCount(const napi_value& value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->cacheCount.set(attribute.value());
        }
    }

    void TaroWaterFlow::SetCacheCount() {
        if (attributes_->cacheCount.has_value()) {
            std::shared_ptr<TaroWaterFlowNode> render = std::static_pointer_cast<TaroWaterFlowNode>(GetHeadRenderNode());
            render->setCacheCount(attributes_->cacheCount.value());
        }
    }

    void TaroWaterFlow::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroScrollerContainer::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::LOWER_THRESHOLD_COUNT:
                GetLowerThresholdCount(value);
                break;
            case ATTRIBUTE_NAME::UPPER_THRESHOLD_COUNT:
                GetUpperThresholdCount(value);
                break;
            case ATTRIBUTE_NAME::CACHE_COUNT:
                GetCacheCount(value);
                if (is_init_) {
                    SetCacheCount();
                }
                break;
            default:
                break;
        }
    }

    void TaroWaterFlow::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(GetHeadRenderNode());
        if (TaroScrollerContainer::attributes_->scrollX.has_value()) {
            auto isScrollX = TaroScrollerContainer::attributes_->scrollX.has_value();
            if (isScrollX) {
                water_flow->setScrollDirection(ARKUI_SCROLL_DIRECTION_HORIZONTAL);
            } else {
                water_flow->setScrollDirection(ARKUI_SCROLL_DIRECTION_VERTICAL);
            }
        } else {
            water_flow->setScrollDirection(ARKUI_SCROLL_DIRECTION_VERTICAL);
        }
        if (TaroScrollerContainer::attributes_->showScrollbar.has_value()) {
            water_flow->setScrollBarDisplay(
                TaroScrollerContainer::attributes_->showScrollbar.value()
                    ? ARKUI_SCROLL_BAR_DISPLAY_MODE_ON
                    : ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
        }
        if (parentType_ != TARO_ELEMENT_MAX) {
            water_flow->setNestedMode(ARKUI_SCROLL_NESTED_MODE_PARENT_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
        }

        if (attributes_->cacheCount.has_value()) {
            water_flow->setCacheCount(attributes_->cacheCount.value());
        }

        water_flow->setFriction(0.85);
        water_flow->SetStyle(style_);

        SetScrollAnimationDuration();
        SetScrollWithAnimation();
    }

    bool TaroWaterFlow::bindListenEvent(const std::string& event_name) {
        if (event_name == "scroll") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_DID_WATER_FLOW_ON_SCROLL, event_name,
                                          [&](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                                              if (auto water_flow_event = std::dynamic_pointer_cast<TaroEvent::TaroEventDidWaterFlowScroll>(event)) {
                                                  const ArkUI_AttributeItem* item = NativeNodeApi::getInstance()->getAttribute(
                                                      GetHeadRenderNode()->GetArkUINodeHandle(), NODE_SCROLL_OFFSET);
                                                  scrollTop_ = item->value[1].f32;
                                                  NapiSetter::SetProperty(water_flow_event->detail(), "scrollTop", scrollTop_);
                                              }
                                              return 0;
                                          });
        } else {
            if (event_name == "scrolltolower") {
                event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
            }
            return TaroScrollerContainer::bindListenEvent(event_name);
        }

        return true;
    }

    void TaroWaterFlow::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(GetHeadRenderNode());
        if (water_flow != nullptr) {
            auto children = child->child_nodes_;
            for (int i = 0; i < children.size(); i++) {
                children[i]->SetParentNode(nullptr);
                water_flow->adapter_->removeItem(children[i]);
            }
            water_flow->initSections(child_nodes_);
        }
        TaroElement::onRemoveChild(child, is_detach_render_node);
    }

    void TaroWaterFlow::onAppendChild(std::shared_ptr<TaroNode> child) {
        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(GetHeadRenderNode());
        if (water_flow != nullptr) {
            auto childElement = std::dynamic_pointer_cast<TaroDOM::TaroFlowSection>(child);
            if (childElement) {
                childElement->BuildProcess();
            }
            water_flow->initSections(child_nodes_);
        }
    }

    void TaroWaterFlow::onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {}

    void TaroWaterFlow::onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(GetHeadRenderNode());
        if (water_flow != nullptr) {
            auto childElement = std::dynamic_pointer_cast<TaroDOM::TaroFlowSection>(child);
            if (childElement) {
                childElement->BuildProcess();
            }
            water_flow->initSections(child_nodes_);
        }
    }

    void TaroWaterFlow::callJSFunc(std::string eventType) {
        // detail 填充
        auto event = std::make_shared<TaroEvent::UserEventBase>(eventType);
        napi_value js_detail = event->detail();

        NapiSetter::SetProperty(js_detail, "deltaX", 0);
        NapiSetter::SetProperty(js_detail, "deltaY", 0);
        getEventEmitter()->triggerEvents(event);
    }

    void TaroWaterFlow::handleOnScrollIndex() {
        std::weak_ptr<TaroNode> node_ref = shared_from_this();
        auto scroll_fun = [node_ref](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            if (auto node = node_ref.lock()) {
                auto water_flow = std::dynamic_pointer_cast<TaroWaterFlowNode>(node->GetHeadRenderNode());
                if (water_flow == nullptr) {
                    TARO_LOG_ERROR("handleOnScrollIndex", "waterflow render node is nullptr");
                    return -1;
                }
                auto comp_event = std::static_pointer_cast<TaroEvent::TaroEventLazyLoadScrollIndex>(event);
                int32_t startIndex = comp_event->startIndex_;
                int32_t endIndex = comp_event->endIndex_;
                TARO_LOG_DEBUG("TaroNodeAdapter", "item handleOnScrollIndex start:%{public}d, end:%{public}d", startIndex, endIndex);
                if (auto water_flow_element = std::dynamic_pointer_cast<TaroWaterFlow>(node)) {
                    water_flow_element->start_index = startIndex;
                    water_flow_element->end_index = endIndex;
                    water_flow_element->handleVisibleNode();

                    if (water_flow_element->attributes_->lowerThresholdCount.has_value()) {
                        int32_t total = water_flow->adapter_->getDataSize();
                        if (endIndex + water_flow_element->attributes_->lowerThresholdCount.value() >= total) {
                            water_flow_element->callJSFunc("scrolltolower");
                        }
                    }
                }
                return 0;
            }
        };
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_WATER_FLOW_ON_SCROLL_INDEX, "scrollindex", scroll_fun);
    }

    int32_t TaroWaterFlow::GetCacheCount() {
        return attributes_->cacheCount.value_or(1);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
