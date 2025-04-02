/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "page_container.h"

#include <arkui/native_node.h>

#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/cssom/stylesheet/pointer_events.h"
#include "runtime/cssom/stylesheet/utils.h"
#include "runtime/dom/ark_nodes/stack.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroPageContainer::TaroPageContainer(napi_value node)
        : TaroElement(node) {}

    TaroPageContainer::~TaroPageContainer() {
        // todo: 继续排查page的headerrender被谁引用了
        if (GetHeadRenderNode()) {
            GetHeadRenderNode()->RemoveChild(GetFooterRenderNode());
        }
    }

    void TaroPageContainer::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            // page 的root节点
            auto render_view = std::make_shared<TaroStackNode>(element);
            render_view->Build();
            // 挂载页面元素的节点
            auto page_view = std::make_shared<TaroStackNode>(element);
            page_view->Build();
            // Fixed 的 root 节点
            auto page_fixed_root = std::make_shared<TaroStackNode>(element);
            page_fixed_root->Build();

            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setPointerEvents(page_fixed_root->GetArkUINodeHandle(),
                                                                            PropertyType::PointerEvents::None);

            // TODO：暂时修复部分场景可能存在fixed的元素position没生效
            page_fixed_root->SetAlign(ARKUI_ALIGNMENT_START); // TODO：暂时修复部分场景可能存在fixed的元素position没生效
            page_view->SetAlign(ARKUI_ALIGNMENT_START);

            SetHeadRenderNode(render_view);
            SetFooterRenderNode(page_view);
            render_view->AppendChild(page_view);
            render_view->AppendChild(page_fixed_root);
            fixed_root_ = page_fixed_root;
        }
    }

    void TaroPageContainer::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto page_path = this->context_->page_path_;
        auto dimension_context = TaroCSSOM::CSSStyleSheet::GetInstance()->GetCurrentPageDimension(page_path);
        auto root_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        root_style->alignItems.set(ARKUI_ITEM_ALIGNMENT_START);
        root_style->width.set(Dimension{dimension_context->viewport_width_});
        Dimension min_height = dimension_context->viewport_height_;
        root_style->minHeight.set(min_height);
        root_style->position.set(PropertyType::Position::Relative);

        auto page_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        page_style->alignItems.set(ARKUI_ITEM_ALIGNMENT_START);
        page_style->width.set(Dimension{Dimension{dimension_context->viewport_width_}});
        page_style->minHeight.set(min_height);
        page_style->position.set(PropertyType::Position::Relative);

        auto fixed_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        fixed_style->width.set(Dimension{Dimension{dimension_context->viewport_width_}});
        fixed_style->height.set(Dimension{Dimension{dimension_context->viewport_height_}});
        fixed_style->position.set(PropertyType::Position::Absolute);

        GetHeadRenderNode()->SetStyle(root_style);
        GetFooterRenderNode()->SetStyle(page_style);
        fixed_root_->SetStyle(fixed_style);
    }

    void TaroPageContainer::NotifyFixedElementVisibility() {
        if (fixed_root_) {
            for (auto fixedItem : fixed_root_->children_refs_) {
                const auto elementRef = fixedItem->element_ref_.lock();
                if (elementRef) {
                    YGDisplay display = elementRef->CheckAncestorsVisibility() ? YGDisplay::YGDisplayFlex : YGDisplay::YGDisplayNone;
                    fixedItem->SetFixedVisibility(display == YGDisplay::YGDisplayNone ? ARKUI_VISIBILITY_HIDDEN : ARKUI_VISIBILITY_VISIBLE);
                }
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime
