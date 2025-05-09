/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "view.h"

#include <arkui/native_node.h>

#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/utils.h"
#include "runtime/dom/ark_nodes/scroll.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/water_flow.h"
#include "runtime/dom/element/list_item_group.h"
#include "runtime/dom/element/scroll_view.h"
#include "runtime/dom/element/water_flow.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_touch.h"
#include "runtime/dom/event/gesture/gesture_param.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroView::TaroView(napi_value node)
        : TaroElement(node) {
    }

    void TaroView::Build() {
        // 构建render node
        if (!is_init_) {
            if (style_ && style_->overflow.has_value() && style_->overflow.value() == PropertyType::Overflow::Scroll) {
                // 动态切换成scroll-view
                BuildScrollView();
            } else {
                auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
                auto render_view = std::make_shared<TaroStackNode>(element);
                SetRenderNode(render_view);
                GetHeadRenderNode()->Build();
                auto parent = std::dynamic_pointer_cast<TaroListItemGroup>(GetParentNode());
                if (parent && tag_name_ == TAG_NAME::STICKY_HEADER) {
                    parent->setStickyHeader(render_view);
                }
            }
            handleOnTouch();
        }
    }

    void TaroView::GetNodeAttributes() {
        TaroElement::GetNodeAttributes();

        GetCatchMoveAttribute(GetAttributeNodeValue("catchMove"));
    }

    void TaroView::CheckTextAlign() {
        if (style_->textAlign.has_value()) {
            auto textAlign = style_->textAlign.value();
            if (textAlign == ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_CENTER || textAlign == ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_END) {
                bool hasTextNode = false;
                for (auto child : child_nodes_) {
                    if (child->node_type_ == TEXT_NODE) {
                        hasTextNode = true;
                        break;
                    }
                }
                if (hasTextNode) {
                    style_->display = PropertyType::Display::Flex;
                    if (style_->flexDirection.has_value() && (style_->flexDirection.value() == ARKUI_FLEX_DIRECTION_COLUMN || style_->flexDirection.value() == ARKUI_FLEX_DIRECTION_COLUMN_REVERSE)) {
                        style_->alignItems = textAlign == ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_CENTER ? ARKUI_ITEM_ALIGNMENT_CENTER : ARKUI_ITEM_ALIGNMENT_END;
                    } else {
                        style_->justifyContent = textAlign == ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_CENTER ? ARKUI_FLEX_ALIGNMENT_CENTER : ARKUI_FLEX_ALIGNMENT_END;
                    }
                }
            }
        }
    }

    void TaroView::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        // 检查文本内容的对齐方式
        CheckTextAlign();
        SetOverflowScrollAttribute();
        GetHeadRenderNode()->SetStyle(style_);
        SetCatchMoveAttribute(GetHeadRenderNode(), GetAttributeNodeValue("catchMove"));
    }

    void TaroView::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);

        // 没有初始化没有 attribute
        if (!is_init_ || GetHeadRenderNode() == nullptr)
            return;

        switch (name) {
            case ATTRIBUTE_NAME::CATCH_MOVE:
                GetCatchMoveAttribute(value);
                if (is_init_) {
                    SetCatchMoveAttribute(renderNode, value);
                }
                break;
            default:
                break;
        }
    }

    bool TaroView::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            // event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::Gesture, (int)TaroEvent::TaroGestureType::Tap, event_name);
            // event_emitter_->gesture()->registerTap(event_name);

        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "touchcancel") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_CANCEL, event_name);
        } else if (event_name == "longpress") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::Gesture, int(TaroEvent::TaroGestureType::LongPress), event_name);
            // event_emitter_->gesture()->registerLongPress(event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroView::handleOnTouch() {
        // catchMove 理论上是阻止当前节点以上所有节点的 TouchMove 事件，所以这里需要做两件事情：
        // 1. 阻止所有容器组件的滚动
        // 2. 阻止以上节点的 touchMove 事件的触发（TODO：需要和强哥商量如何阻止）
        auto touch_start_fun = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            bool has_catch_move = false;
            TraverseAncestor([has_catch_move, this](std::shared_ptr<TaroNode> parentNode) mutable {
                if (auto view = std::dynamic_pointer_cast<TaroView>(parentNode)) {
                    if (view->attributes_->catchMove.has_value() && view->attributes_->catchMove.value() == true) {
                        has_catch_move = true;
                    }
                }

                if (has_catch_move) {
                    if (auto scroll_view = std::dynamic_pointer_cast<TaroScrollView>(parentNode)) {
                        catch_container_list_.push_back(scroll_view);
                        std::static_pointer_cast<TaroScrollNode>(scroll_view->GetHeadRenderNode())->setScrollEnableScrollInteraction(false);
                    } else if (auto water_flow = std::dynamic_pointer_cast<TaroWaterFlow>(parentNode)) {
                        catch_container_list_.push_back(water_flow);
                        std::static_pointer_cast<TaroWaterFlowNode>(water_flow->GetHeadRenderNode())->setScrollEnableScrollInteraction(false);
                    }
                }

                return false;
            });
            return 0;
        };
        auto touch_end_fun = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            if (!catch_container_list_.empty()) {
                std::for_each(catch_container_list_.begin(), catch_container_list_.end(), [](const auto& elem) {
                    auto element = elem.lock();
                    if (element) {
                        switch (element->tag_name_) {
                            case TaroRuntime::TAG_NAME::SCROLL_VIEW:
                                std::static_pointer_cast<TaroScrollNode>(element->GetHeadRenderNode())->setScrollEnableScrollInteraction(true);
                                break;
                            case TaroRuntime::TAG_NAME::WATER_FLOW:
                                std::static_pointer_cast<TaroWaterFlowNode>(element->GetHeadRenderNode())->setScrollEnableScrollInteraction(true);
                                break;
                            default:
                                break;
                        }
                    }
                });
                catch_container_list_.clear();
            }
            return 0;
        };
        event_emitter_->registerEvent_NoCallBackWithBubble(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, "touchstart", touch_start_fun);
        event_emitter_->registerEvent_NoCallBackWithBubble(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, "touchend", touch_end_fun);
    }

    void TaroView::Build(std::shared_ptr<TaroElement>& reuse_element) {
        std::shared_ptr<TaroStackNode> new_node = std::dynamic_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            // 往下滑，节点没有被 init 过，需要创建新的renderNode
            new_node = std::make_shared<TaroStackNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            SetRenderNode(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->event_emitter_->clearNodeEvent(ark_handle);
            reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
            return;
        }
        if (ark_handle == nullptr) {
            // 节点已经 init 过了，有 renderNode 没有 arkNode，该节点被复用，当前缓存池没有可用节点，需要重新创建新的 arkNode
            auto parent = new_node->parent_ref_.lock();
            if (parent) {
                new_node->Build();
                parent->UpdateChild(new_node);
                updateListenEvent();
                new_node->ClearDifferOldStyleFromElement();
            }
            return;
        }
        // 往上滑动，节点已经 init 过了，有 renderNode 没有 arkNode，该节点被复用。
        new_node->SetArkUINodeHandle(ark_handle);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        reuse_element->event_emitter_->clearNodeEvent(ark_handle);
        updateListenEvent();
        reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
    }

    void TaroView::BuildScrollView() {
        if (!is_init_) {
            // 需要构建scrollNode
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_scroll_view = std::make_shared<TaroScrollNode>(element);
            render_scroll_view->Build();
            render_scroll_view->SetShouldPosition(false);

            // 渲染column/row的节点
            auto render_view = std::make_shared<TaroStackNode>(element);
            render_view->Build();

            render_scroll_view->AppendChild(render_view);
            SetHeadRenderNode(render_scroll_view);
            SetFooterRenderNode(render_view);
        }
    }

    void TaroView::SetOverflowScrollAttribute() {
        if (auto render_scroll_view = std::dynamic_pointer_cast<TaroScrollNode>(GetHeadRenderNode())) {
            if (!style_->flexShrink.has_value()) {
                render_scroll_view->SetFlexShrink(0);
            }
            auto render_view = GetFooterRenderNode();

            // 判断方向
            auto arkUI_ScrollDirection = ARKUI_SCROLL_DIRECTION_VERTICAL;
            if (style_->display.has_value() && style_->display.value() == PropertyType::Display::Flex) {
                if (style_->flexDirection.has_value()) {
                    if (
                        style_->flexDirection.value() == ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN ||
                        style_->flexDirection.value() == ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN_REVERSE) {
                        arkUI_ScrollDirection = ARKUI_SCROLL_DIRECTION_VERTICAL;
                    } else {
                        arkUI_ScrollDirection = ARKUI_SCROLL_DIRECTION_HORIZONTAL;
                    }
                } else {
                    arkUI_ScrollDirection = ARKUI_SCROLL_DIRECTION_HORIZONTAL;
                }
            }
            render_scroll_view->setScrollDirection(arkUI_ScrollDirection);

            auto viewStyle = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
            viewStyle->display = style_->display;
            viewStyle->alignItems = style_->alignItems;
            viewStyle->flexWrap = style_->flexWrap;
            render_view->SetStyle(viewStyle);
            render_view->SetFlexShrink(0);
            render_view->SetFlexGrow(1);
            // 设置默认样式
            if (arkUI_ScrollDirection == ARKUI_SCROLL_DIRECTION_HORIZONTAL) {
                render_view->SetFlexDirection(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW);

                if (style_->height.has_value() && style_->height.value().Unit() != DimensionUnit::CALC) {
                    render_view->SetHeight(style_->height.value(), Dimension{0, DimensionUnit::NONE});
                    YGNodeStyleSetOverflow(GetHeadRenderNode()->ygNodeRef, YGOverflowScroll);
                } else {
                    render_view->SetHeight(Dimension{0, DimensionUnit::AUTO}, Dimension{0, DimensionUnit::NONE});
                }
                render_view->SetWidth(Dimension{0, DimensionUnit::AUTO});
                // 设置约束，防止内部的元素百分比无法依赖到确定的值
                render_view->SetMaxHeight(Dimension{1, DimensionUnit::PERCENT});
            } else {
                render_view->SetFlexDirection(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN);

                // 宽度自适应的话
                if (style_->width.has_value() && style_->width.value().Unit() != DimensionUnit::AUTO) {
                    YGNodeStyleSetOverflow(GetHeadRenderNode()->ygNodeRef, YGOverflowScroll);
                }
                if (style_->width.has_value() && style_->width.value().Unit() != DimensionUnit::CALC) {
                    render_view->SetWidth(style_->width.value());
                } else {
                    if (style_->display.has_value() && style_->display.value() == PropertyType::Display::Flex) {
                        render_view->SetWidth(Dimension{0, DimensionUnit::AUTO});
                        YGNodeStyleSetOverflow(GetHeadRenderNode()->ygNodeRef, YGOverflowVisible);
                    } else {
                        // 当没有设置弹性布局且滚动方向是纵向时，width默认是撑满的
                        render_view->SetWidth(Dimension{1, DimensionUnit::PERCENT});
                    }
                }
                render_view->SetHeight(Dimension{0, DimensionUnit::AUTO}, Dimension{0, DimensionUnit::NONE});
                // 设置约束，防止内部的元素百分比无法依赖到确定的值
                render_view->SetMaxWidth(Dimension{1, DimensionUnit::PERCENT});
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime
