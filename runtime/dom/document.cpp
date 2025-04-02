/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "document.h"

#include "runtime/constant.h"
#include "runtime/dom/element/button.h"
#include "runtime/dom/element/canvas.h"
#include "runtime/dom/element/checkbox.h"
#include "runtime/dom/element/checkbox_group.h"
#include "runtime/dom/element/ets_component.h"
#include "runtime/dom/element/flow_item.h"
#include "runtime/dom/element/flow_section.h"
#include "runtime/dom/element/icon.h"
#include "runtime/dom/element/image.h"
#include "runtime/dom/element/input.h"
#include "runtime/dom/element/label.h"
#include "runtime/dom/element/list.h"
#include "runtime/dom/element/list_item.h"
#include "runtime/dom/element/list_item_group.h"
#include "runtime/dom/element/movable_area.h"
#include "runtime/dom/element/movable_view.h"
#include "runtime/dom/element/page_container.h"
#include "runtime/dom/element/picker.h"
#include "runtime/dom/element/progress.h"
#include "runtime/dom/element/radio.h"
#include "runtime/dom/element/radio_group.h"
#include "runtime/dom/element/scroll_view.h"
#include "runtime/dom/element/slider.h"
#include "runtime/dom/element/swiper.h"
#include "runtime/dom/element/swiper_item.h"
#include "runtime/dom/element/text.h"
#include "runtime/dom/element/text_area.h"
#include "runtime/dom/element/view.h"
#include "runtime/dom/element/water_flow.h"
#include "runtime/dom/event/event_visible.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroDocument::TaroDocument(napi_value node)
        : TaroElement(node) {
        node_type_ = NODE_TYPE::DOCUMENT_NODE;
    }

    std::shared_ptr<TaroPageContainer> TaroDocument::findDOMPage(std::shared_ptr<TaroElement> el) {
        std::shared_ptr<TaroContext> context = el->context_;
        if (context) {
            std::shared_ptr<TaroPageContainer> page = context->page_element_.lock();
            if (app_) {
                auto pageList = app_->child_nodes_;
                auto it = std::find(pageList.begin(), pageList.end(), page);
                if (it != pageList.end()) {
                    return page;
                }
            }

            if (entry_async_) {
                auto entryAsyncPageList = entry_async_->child_nodes_;
                auto it = std::find(entryAsyncPageList.begin(), entryAsyncPageList.end(), page);
                if (it != entryAsyncPageList.end()) {
                    return page;
                }
            }
        }
        return nullptr;
    }

    std::shared_ptr<TaroElement> TaroDocument::CreateElement(napi_value& node) {
        TAG_NAME tag_name_ = TaroDOM::TaroElement::GetTagName(node);
        NODE_TYPE node_type_ = TaroDOM::TaroNode::GetNodeType(node);
        std::shared_ptr<TaroDOM::TaroElement> item;

        if (node_type_ == NODE_TYPE::TEXT_NODE) {
            tag_name_ = TAG_NAME::TEXT;
        } else if (node_type_ == NODE_TYPE::DOCUMENT_NODE) {
            tag_name_ = TAG_NAME::DOCUMENT;
        }

        switch (tag_name_) {
            case TAG_NAME::PAGE: {
                item = std::make_shared<TaroDOM::TaroPageContainer>(node);
                break;
            }
            case TAG_NAME::SCROLL_VIEW: {
                item = std::make_shared<TaroDOM::TaroScrollView>(node);
                break;
            }
            case TAG_NAME::IMAGE: {
                item = std::make_shared<TaroDOM::TaroImage>(node);
                break;
            }
            case TAG_NAME::SPAN:
            case TAG_NAME::TEXT: {
                item = std::make_shared<TaroDOM::TaroText>(node);
                break;
            }
            case TAG_NAME::SWIPER: {
                item = std::make_shared<TaroDOM::TaroSwiper>(node);
                break;
            }
            case TAG_NAME::SWIPER_ITEM: {
                item = std::make_shared<TaroDOM::TaroSwiperItem>(node);
                break;
            }
            case TAG_NAME::BUTTON: {
                item = std::make_shared<TaroDOM::TaroButton>(node);
                break;
            }
            case TAG_NAME::INPUT: {
                item = std::make_shared<TaroDOM::TaroInput>(node);
                break;
            }
            case TAG_NAME::ICON: {
                item = std::make_shared<TaroDOM::TaroIcon>(node);
                break;
            }
            case TAG_NAME::MOVABLE_AREA: {
                item = std::make_shared<TaroDOM::TaroMovableArea>(node);
                break;
            }
            case TAG_NAME::MOVABLE_VIEW: {
                item = std::make_shared<TaroDOM::TaroMovableView>(node);
                break;
            }
            case TAG_NAME::LIST: {
                item = std::make_shared<TaroDOM::TaroList>(node);
                break;
            }
            case TAG_NAME::LIST_ITEM: {
                item = std::make_shared<TaroDOM::TaroListItem>(node);
                break;
            }
            case TAG_NAME::WATER_FLOW: {
                item = std::make_shared<TaroDOM::TaroWaterFlow>(node);
                break;
            }
            case TAG_NAME::FLOW_SECTION: {
                item = std::make_shared<TaroDOM::TaroFlowSection>(node);
                break;
            }
            case TAG_NAME::FLOW_ITEM: {
                item = std::make_shared<TaroDOM::TaroFlowItem>(node);
                break;
            }
            case TAG_NAME::PICKER: {
                item = std::make_shared<TaroDOM::TaroPicker>(node);
                break;
            }
            case TAG_NAME::PROGRESS: {
                item = std::make_shared<TaroDOM::TaroProgress>(node);
                break;
            }
            case TAG_NAME::STICKY_SECTION: {
                item = std::make_shared<TaroDOM::TaroListItemGroup>(node);
                break;
            }
            case TAG_NAME::STICKY_HEADER: {
                item = std::make_shared<TaroDOM::TaroView>(node);
                break;
            }
            case TAG_NAME::CHECKBOX: {
                item = std::make_shared<TaroDOM::TaroCheckbox>(node);
                break;
            }
            case TAG_NAME::CHECKBOX_GROUP: {
                item = std::make_shared<TaroDOM::TaroCheckboxGroup>(node);
                break;
            }
            case TAG_NAME::RADIO: {
                item = std::make_shared<TaroDOM::TaroRadio>(node);
                break;
            }
            case TAG_NAME::RADIO_GROUP: {
                item = std::make_shared<TaroDOM::TaroRadioGroup>(node);
                break;
            }
            case TAG_NAME::LABEL: {
                item = std::make_shared<TaroDOM::TaroLabel>(node);
                break;
            }
            case TAG_NAME::CANVAS: {
                item = std::make_shared<TaroDOM::TaroCanvas>(node);
                break;
            }
            case TAG_NAME::VIEW:
            case TAG_NAME::FORM:
            case TAG_NAME::DOCUMENT:
            case TAG_NAME::HTML:
            case TAG_NAME::HEAD:
            case TAG_NAME::BODY:
            case TAG_NAME::CONTAINER:
            case TAG_NAME::APP:
            case TAG_NAME::ENTRY_ASYNC: {
                item = std::make_shared<TaroDOM::TaroView>(node);
                break;
            }
            case TAG_NAME::SLIDER: {
                item = std::make_shared<TaroDOM::TaroSlider>(node);
                break;
            }
            case TAG_NAME::TEXTAREA: {
                item = std::make_shared<TaroDOM::TaroTextArea>(node);
                break;
            }
            case TAG_NAME::UNKNOWN:
            default: {
                item = std::make_shared<TaroDOM::TaroEtsComponent>(node);
            }
                item->getEventEmitter();
        }

        // Note: 不能放在 TaroNode 构造函数里
        AttachTaroNode(item);

#if IS_DEBUG
        TARO_LOG_DEBUG(
            "TaroDocument",
            "nodeName: %{public}s nodeType: %{public}d nid: %{public}d",
            item->GetNodeName().c_str(), item->GetNodeType(),
            item->GetNodeId());
#endif
        return item;
    }

    std::shared_ptr<TaroElement> TaroDocument::GetElementById(int32_t id) {
        auto it = taro_nodes_.find(id);
        if (it != taro_nodes_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void TaroDocument::AttachTaroNode(std::shared_ptr<TaroElement> node) {
        if (node != nullptr) {
#if IS_DEBUG
            TARO_LOG_DEBUG(
                "TaroDocument",
                "nodeName: %{public}s nodeType: %{public}d nid: %{public}d",
                node->GetNodeName().c_str(), node->GetNodeType(),
                node->GetNodeId());
#endif
            taro_nodes_[node->nid_] = node;
            switch (node->tag_name_) {
                case TAG_NAME::BODY:
                    body_ = node;
                    break;
                case TAG_NAME::APP:
                    app_ = node;
                    break;
                case TAG_NAME::ENTRY_ASYNC:
                    entry_async_ = node;
                    break;
                default:
                    break;
            }
        }
    }

    void TaroDocument::DetachTaroNode(std::shared_ptr<TaroNode> node) {
        if (node != nullptr) {
            TARO_LOG_DEBUG("TaroDocument", "nid: %{public}d", node->nid_);
            auto taroElement = std::static_pointer_cast<TaroElement>(node);
            VisibleEventListener::GetInstance()->Disconnect(taroElement);
            taro_nodes_.erase(node->nid_);
        }
    }

    void TaroDocument::DetachTaroNode(int32_t id) {
        if (id != 0) {
            TARO_LOG_DEBUG("TaroDocument", "nid: %{public}d", id);
            std::shared_ptr<TaroElement> node = GetElementById(nid_);
            DetachTaroNode(node);
        }
    }

    void TaroDocument::DetachAllTaroNode() {
        // Note: 释放页面节点
        if (app_) {
            DetachTaroNode(app_->nid_);
        }
        if (entry_async_) {
            DetachTaroNode(entry_async_->nid_);
        }
        // Note: 获取野节点释放，直接 clear 会导致 map 不稳定导致错误抛出
        std::vector<int32_t> nid_list;
        for (auto el : taro_nodes_) {
            nid_list.push_back(el.first);
        }
        for (auto nid : nid_list) {
            DetachTaroNode(nid);
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
