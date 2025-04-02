/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./custom.h"

#include "helper/TaroTimer.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroCustomNode::TaroCustomNode(TaroElementRef element)
        : TaroRenderNode(element) {};

    TaroCustomNode::~TaroCustomNode() {};

    void TaroCustomNode::OnMeasure(ArkUI_NodeCustomEvent *event) {
        SystraceSection s("Taro Custom Layout:: OnMeasure");

        if (style_ref_) {
            bool useYogaMeasure = style_ref_->width.has_value() && style_ref_->height.has_value();
            if (!useYogaMeasure) {
                if (style_ref_->display.has_value() && style_ref_->display.value() == PropertyType::Display::None) {
                    useYogaMeasure = true;
                } else if (style_ref_->visibility.has_value()) {
                    auto visibility = style_ref_->visibility.value();
                    useYogaMeasure = (visibility == ARKUI_VISIBILITY_HIDDEN || visibility == ARKUI_VISIBILITY_NONE);
                }
            }
            if (useYogaMeasure) {
                return TaroRenderNode::OnMeasure(event);
            }
        }

        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();

        if (parent_ref_.expired()) return;
        auto parent_differ_ = parent_ref_.lock()->layoutDiffer_;
        auto layoutConstrain = OH_ArkUI_NodeCustomEvent_GetLayoutConstraintInMeasure(event);
        // 创建子节点布局限制，复用父组件布局中的百分比参考值。
        auto childLayoutConstrain = OH_ArkUI_LayoutConstraint_Copy(layoutConstrain);
        if (OH_ArkUI_LayoutConstraint_GetMaxHeight(childLayoutConstrain) <= 0) {
            OH_ArkUI_LayoutConstraint_SetMaxHeight(childLayoutConstrain, static_cast<int32_t>(vp2Px(parent_differ_.computed_style_.height)));
        }
        if (OH_ArkUI_LayoutConstraint_GetMaxWidth(childLayoutConstrain) <= 0) {
            OH_ArkUI_LayoutConstraint_SetMaxWidth(childLayoutConstrain, static_cast<int32_t>(vp2Px(parent_differ_.computed_style_.width)));
        }
        OH_ArkUI_LayoutConstraint_SetMinHeight(childLayoutConstrain, 0);
        OH_ArkUI_LayoutConstraint_SetMinWidth(childLayoutConstrain, 0);

        auto ark_node_ = GetArkUINodeHandle();
        // 测算子节点获取子节点最大值。
        auto totalSize = nativeNodeApi->getTotalChildCount(ark_node_);
        float width = layoutDiffer_.computed_style_.width;
        float height = layoutDiffer_.computed_style_.height;
        for (uint32_t i = 0; i < totalSize; i++) {
            // auto child = nativeNodeApi->getChildAt(ark_node_, i); // Note: 当前部分组件会返回空指针，暂时不使用该方法
            auto child = children_refs_[i]->GetArkUINodeHandle();
            // 调用测算接口测算子组件。
            nativeNodeApi->measureNode(child, childLayoutConstrain);
            const ArkUI_AttributeItem *margin = nativeNodeApi->getAttribute(child, NODE_MARGIN);
            auto size = nativeNodeApi->getMeasuredSize(child);
            // TARO_LOG_DEBUG("TaroCustomNode", "nid: %{public}d margin top:%{public}f left:%{public}f", element_nid_, margin->value[0].f32, margin->value[2].f32);
            if (size.width && style_ref_ && !style_ref_->width.has_value() && !layoutDiffer_.computed_style_.width) {
                width = px2Vp(size.width);
                float diff = margin->size == 4 ? margin->value[1].f32 + margin->value[3].f32 : margin->value[0].f32 * 2;
                if (diff != std::floor(diff)) {
                    width += diff;
                } else {
                    width += px2Vp(diff);
                }
                SetWidth(width);
            }
            if (size.height && style_ref_ && !style_ref_->height.has_value() && !layoutDiffer_.computed_style_.height) {
                height = px2Vp(size.height);
                float diff = margin->size == 4 ? margin->value[0].f32 + margin->value[2].f32 : margin->value[0].f32 * 2;
                if (diff != std::floor(diff)) {
                    height += diff;
                } else {
                    height += px2Vp(diff);
                }
                SetHeight(height);
            }
        }

        // TARO_LOG_DEBUG("TaroCustomNode", "nid: %{public}s size w:%{public}d h:%{public}d",
        //     element_ref_.lock()->GetNodeName().c_str(), static_cast<int32_t>(vp2Px(width)), static_cast<int32_t>(vp2Px(height)));
        // 自定义测算为所有子节点大小加固定边距。
        nativeNodeApi->setMeasuredSize(
            ark_node_,
            static_cast<int32_t>(vp2Px(width)),
            static_cast<int32_t>(vp2Px(height)));
    };

    void TaroCustomNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_CUSTOM));
        SetCustomLayout();
    };
} // namespace TaroDOM
} // namespace TaroRuntime
