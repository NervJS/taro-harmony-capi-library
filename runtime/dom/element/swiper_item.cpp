//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "swiper_item.h"

#include <arkui/native_node.h>

#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/utils.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/swiper_item.h"
#include "runtime/dom/element/swiper.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"

namespace TaroRuntime::TaroDOM {
TaroSwiperItem::TaroSwiperItem(napi_value node)
    : TaroView(node) {
}

void TaroSwiperItem::Build() {
    // 构建render node
    if (!is_init_) {
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());

        auto render_swiper_item = std::make_shared<TaroSwiperItemNode>(element);
        auto render_view = std::make_shared<TaroStackNode>(element);
        SetHeadRenderNode(render_swiper_item);
        SetFooterRenderNode(render_view);
        render_swiper_item->Build();
        render_view->Build();
        render_swiper_item->AppendChild(render_view);
    }
}

void TaroSwiperItem::SetAttributesToRenderNode() {
    TaroView::SetAttributesToRenderNode();

    GetHeadRenderNode()->SetWidth(Dimension{1, DimensionUnit::PERCENT});
    GetHeadRenderNode()->SetHeight(Dimension{1, DimensionUnit::PERCENT});

    GetFooterRenderNode()->SetStyle(style_);
}

} // namespace TaroRuntime::TaroDOM
