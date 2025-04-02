/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "flow_item.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/dom/element/flow_section.h"
#include "water_flow.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroFlowItemNode::TaroFlowItemNode(const TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroFlowItemNode::~TaroFlowItemNode() {}

    void TaroFlowItemNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_FLOW_ITEM));
    }

    void TaroFlowItemNode::Layout() {
        if (!is_first_layout_finish_) {
            TaroRenderNode::Layout();
            return;
        }
        auto width_old = layoutDiffer_.computed_style_.width;
        auto height_old = layoutDiffer_.computed_style_.height;
        TaroRenderNode::Layout();
        if (element_ref_.lock() && element_ref_.lock()->is_reused_) {
            return;
        }
        auto width_new = layoutDiffer_.computed_style_.width;
        auto height_new = layoutDiffer_.computed_style_.height;
        if (!NearEqual(width_old, width_new, 1.0f) || !NearEqual(height_old, height_new, 1.0f)) {
            auto parent = std::dynamic_pointer_cast<TaroWaterFlowNode>(parent_ref_.lock());
            if (parent) {
                parent->adapter_->reloadItem(element_ref_.lock());
            }
        }
    }

    void TaroFlowItemNode::LayoutSelf() {
        if (auto childElement = element_ref_.lock()) {
            if (auto flowSection = std::dynamic_pointer_cast<TaroFlowSection>(childElement->GetParentNode())) {
                if (auto waterFlow = std::static_pointer_cast<TaroWaterFlowNode>(parent_ref_.lock())) {
                    TaroSetCanMeasureChild(waterFlow->ygNodeRef, true);
                    float availableWidth = waterFlow->layoutDiffer_.computed_style_.width - waterFlow->layoutDiffer_.computed_style_.paddingLeft - waterFlow->layoutDiffer_.computed_style_.paddingRight;

                    int32_t column = flowSection->column_;
                    double columnGap = flowSection->columnGap_.value_or(0.0);
                    double marginLeft = flowSection->margin_.value_or(SectionMargin()).marginLeft;
                    double marginRight = flowSection->margin_.value_or(SectionMargin()).marginLeft;
                    // step 1 计算可用宽搞
                    float itemAvailableWidth = (availableWidth - (column - 1) * columnGap - marginLeft - marginRight) / column;
                    float itemAvailableHeight = YGUndefined;
                    TaroYogaApi::getInstance()->calcYGLayout(ygNodeRef, itemAvailableWidth, itemAvailableHeight);
                    YGNodeSetHasNewLayout(ygNodeRef, true);
                    bool layoutWithoutDiff = false;
                    if (childElement->reusable_) {
                        layoutWithoutDiff = true;
                    }
                    LayoutAll(layoutWithoutDiff);

                    TaroSetCanMeasureChild(waterFlow->ygNodeRef, false);
                }
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime