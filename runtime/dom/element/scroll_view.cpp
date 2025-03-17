//
// Created on 2024/4/17.
//

#include "scroll_view.h"

#include "helper/SwitchManager.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/ark_nodes/scroll.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_scroll.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroScrollView::TaroScrollView(napi_value node)
        : TaroScrollerContainer(node) {}

    void TaroScrollView::Build() {
        // 构建render node
        if (!is_init_) {
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

            TaroScrollerContainer::Build();
            TaroScrollerContainer::handleOnScroll();
            TaroScrollerContainer::findParentType();
        }
    }

    void TaroScrollView::SetScrollYAttribute() {
        SetScrollXAttribute();
    }

    void TaroScrollView::SetScrollXAttribute() {
        TaroScrollerContainer::SetScrollXAttribute();
        auto render_view = std::static_pointer_cast<TaroStackNode>(GetFooterRenderNode());
        is_scroll_x = IsScrollX();
        if (!style_->flexShrink.has_value()) {
            GetHeadRenderNode()->SetFlexShrink(0);
        }
        render_view->SetFlexGrow(1);
        render_view->SetFlexShrink(0);
    
        if (is_scroll_x) {
            render_view->SetFlexDirection(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW);

            if (style_->height.has_value() && style_->height.value().Unit() != DimensionUnit::CALC) {
                render_view->SetHeight(style_->height.value(), Dimension{0, DimensionUnit::NONE});
                overflowMode_ = PropertyType::Overflow::Scroll;
                GetHeadRenderNode()->SetOverflow(overflowMode_);
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
                overflowMode_ = PropertyType::Overflow::Scroll;
                GetHeadRenderNode()->SetOverflow(overflowMode_);
            }
            if (style_->width.has_value() && style_->width.value().Unit() != DimensionUnit::CALC) {
                render_view->SetWidth(style_->width.value());
            } else {
                if (style_->display.has_value() && style_->display.value() == PropertyType::Display::Flex) {
                    render_view->SetWidth(Dimension{0, DimensionUnit::AUTO});
                    overflowMode_ = PropertyType::Overflow::Visible;
                    GetHeadRenderNode()->SetOverflow(overflowMode_);
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

} // namespace TaroDOM
} // namespace TaroRuntime