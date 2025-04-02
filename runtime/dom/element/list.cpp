/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "list.h"

#include "runtime/NapiSetter.h"
#include "runtime/dom/ark_nodes/list.h"
#include "runtime/dom/element/scroller_container.h"
#include "runtime/dom/event/event_hm/event_types/event_scroll.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroList::TaroList(napi_value node)
        : TaroScrollerContainer(node), attributes_(std::make_unique<TaroListAttributes>()) {
        lazy_container = true;
    }

    TaroList::~TaroList() {}

    void TaroList::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_list = std::make_shared<TaroListNode>(element);
            render_list->Build();
            render_list->SetShouldPosition(false);
            SetRenderNode(render_list);
            // 初始化数据源
            for (auto it = element->child_nodes_.begin(); it != element->child_nodes_.end(); ++it) {
                render_list->adapter_->insertItem(*it);
            }
            lazy_node = true;
            handleOnScrollIndex();
            findParentType();
            TaroScrollerContainer::Build();
        }
    }

    void TaroList::GetLowerThresholdCount(const napi_value &value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->lowerThresholdCount.set(attribute.value());
        }
    }

    void TaroList::GetUpperThresholdCount(const napi_value &value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->upperThresholdCount.set(attribute.value());
        }
    }

    void TaroList::GetCacheCount(const napi_value &value) {
        NapiGetter getter(value);
        auto attribute = getter.Int32();
        if (attribute.has_value()) {
            attributes_->cacheCount.set(attribute.value());
        }
    }

    void TaroList::SetCacheCount() {
        if (attributes_->cacheCount.has_value()) {
            std::shared_ptr<TaroListNode> render = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
            render->setCacheCount(attributes_->cacheCount.value());
        }
    }

    void TaroList::GetStickyHeader(const napi_value &value) {
        NapiGetter getter(value);
        auto attribute = getter.BoolNull();
        if (attribute.has_value()) {
            attributes_->stickyHeader.set(attribute.value());
        }
    }

    void TaroList::SetStickyHeader() {
        if (attributes_->stickyHeader.has_value()) {
            std::shared_ptr<TaroListNode> render = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
            ArkUI_StickyStyle style = attributes_->stickyHeader.value() ? ARKUI_STICKY_STYLE_HEADER : ARKUI_STICKY_STYLE_NONE;
            render->setStickyStyle(style);
        }
    }

    void TaroList::GetSpace(const napi_value &value) {
        NapiGetter getter(value);
        TaroHelper::Optional<double> val = getter.Double();
        if (val.has_value()) {
            attributes_->space.set(val.value());
        }
    }

    void TaroList::SetSpace() {
        if (attributes_->space.has_value()) {
            std::shared_ptr<TaroListNode> render = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
            render->setSpace(attributes_->space.value());
        }
    }

    void TaroList::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
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
            case ATTRIBUTE_NAME::STICKY_HEADER:
                GetStickyHeader(value);
                if (is_init_) {
                    SetStickyHeader();
                }
                break;
            case ATTRIBUTE_NAME::SPACE:
                GetSpace(value);
                if (is_init_) {
                    SetSpace();
                }
                break;
            default:
                break;
        }
    }

    void TaroList::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto render_list = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
        if (TaroScrollerContainer::attributes_->scrollX.has_value()) {
            auto isScrollX = TaroScrollerContainer::attributes_->scrollX.has_value();
            if (isScrollX) {
                render_list->setScrollDirection(ARKUI_SCROLL_DIRECTION_HORIZONTAL);
            } else {
                render_list->setScrollDirection(ARKUI_SCROLL_DIRECTION_VERTICAL);
            }
        } else {
            render_list->setScrollDirection(ARKUI_SCROLL_DIRECTION_VERTICAL);
        }
        if (TaroScrollerContainer::attributes_->showScrollbar.has_value()) {
            render_list->setScrollBarDisplay(
                TaroScrollerContainer::attributes_->showScrollbar.value()
                    ? ARKUI_SCROLL_BAR_DISPLAY_MODE_ON
                    : ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
        }
        if (parentType_ != TARO_ELEMENT_MAX) {
            render_list->setNestedMode(ARKUI_SCROLL_NESTED_MODE_PARENT_FIRST, ARKUI_SCROLL_NESTED_MODE_SELF_FIRST);
        }
        SetStickyHeader();
        SetSpace();

        render_list->setFriction(0.85);
        render_list->SetStyle(style_);
    }

    bool TaroList::bindListenEvent(const std::string &event_name) {
        if (event_name == "scroll") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_DID_LIST_ON_SCROLL, event_name,
                                          [&](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value &) -> int {
                                              if (auto scroll_event = std::dynamic_pointer_cast<TaroEvent::TaroEventDidWaterFlowScroll>(event)) {
                                                  const ArkUI_AttributeItem *item = NativeNodeApi::getInstance()->getAttribute(
                                                      GetHeadRenderNode()->GetArkUINodeHandle(), NODE_SCROLL_OFFSET);
                                                  scrollTop_ = item->value[1].f32;
                                                  NapiSetter::SetProperty(scroll_event->detail(), "scrollTop", scrollTop_);
                                              }
                                              return 0;
                                          });
        } else {
            return TaroScrollerContainer::bindListenEvent(event_name);
        }
        return true;
    }

    void TaroList::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        auto render_list = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
        if (render_list != nullptr) {
            render_list->adapter_->removeItem(child);
        }
        TaroElement::onRemoveChild(child, true);
    }

    void TaroList::onAppendChild(std::shared_ptr<TaroNode> child) {
        TaroElement::onAppendChild(child);

        auto render_list = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
        if (render_list != nullptr) {
            render_list->adapter_->insertItem(child);
        }
    }

    void TaroList::onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {
        TaroElement::onReplaceChild(newChild, oldChild);

        auto render_list = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
        if (render_list != nullptr) {
            render_list->adapter_->reloadItem(newChild, oldChild);
        }
    }

    void TaroList::onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        TaroElement::onInsertBefore(child, refChild);

        auto render_list = std::static_pointer_cast<TaroListNode>(GetHeadRenderNode());
        if (render_list != nullptr) {
            render_list->adapter_->insertItemBefore(child, refChild);
        }
    }

    void TaroList::handleOnScrollIndex() {
        std::weak_ptr<TaroNode> node_ref = shared_from_this();
        auto scroll_fun = [node_ref](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value &) -> int {
            if (auto node = node_ref.lock()) {             
                auto list = std::dynamic_pointer_cast<TaroListNode>(node->GetHeadRenderNode());
                if (list == nullptr) {
                    TARO_LOG_ERROR("handleOnScrollIndex", "list render node is nullptr");
                    return -1;
                }
                if (auto list_element = std::dynamic_pointer_cast<TaroList>(node)) {
                    auto comp_event = std::static_pointer_cast<TaroEvent::TaroEventLazyLoadScrollIndex>(event);
                    list_element->start_index = comp_event->startIndex_;
                    list_element->end_index = comp_event->endIndex_;
                    list_element->handleVisibleNode();
                }
            }
            return 0;
        };
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_LIST_ON_SCROLL_INDEX, "scrollindex", scroll_fun);
    }

    int32_t TaroList::GetCacheCount() {
        return attributes_->cacheCount.value_or(1);
    }
} // namespace TaroDOM
} // namespace TaroRuntime