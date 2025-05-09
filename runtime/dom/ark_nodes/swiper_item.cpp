/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./swiper_item.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime::TaroDOM {
TaroSwiperItemNode::TaroSwiperItemNode(const TaroElementRef element)
    : TaroStackNode(element) {
    textNodeStyle_ = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
}

TaroSwiperItemNode::~TaroSwiperItemNode() {}

void TaroSwiperItemNode::SetStyle(StylesheetRef style_ref) {
    if (style_ref) {
        style_ref->alignSelf.set(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_STRETCH);
        style_ref->width.set(Dimension{1, DimensionUnit::PERCENT});
        style_ref->height.set(Dimension{1, DimensionUnit::PERCENT});
        TaroStackNode::SetStyle(style_ref);
    } else {
        SetWidth(Dimension{1, DimensionUnit::PERCENT});
        SetHeight(Dimension{1, DimensionUnit::PERCENT});
    }
}
void TaroSwiperItemNode::Build() {
    TaroStackNode::Build();
}

void TaroSwiperItemNode::Measure() {
    if (auto parent = parent_ref_.lock()) {
        if (auto swiper = std::dynamic_pointer_cast<TaroSwiperNode>(parent)) {
            ChildAvailableSize availableSize = swiper->GetChildAvailableSize();
            TaroYogaApi::getInstance()->calcYGLayout(ygNodeRef, availableSize.width, availableSize.height);
        }
    }
};
} // namespace TaroRuntime::TaroDOM
