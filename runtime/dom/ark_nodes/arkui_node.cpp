/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "arkui_node.h"

#include <cstdint>
#include <stack>
#include <math.h>
#include <native_vsync/native_vsync.h>

#include "helper/ImageLoader.h"
#include "helper/TaroTimer.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/animation/node_animations.h"
#include "runtime/dom/ark_nodes/list.h"
#include "runtime/dom/ark_nodes/scroll.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "runtime/dom/ark_nodes/water_flow.h"
#include "runtime/dom/element/flow_section.h"
#include "runtime/dom/element/list.h"
#include "runtime/dom/element/list_item.h"
#include "runtime/dom/element/list_item_group.h"
#include "runtime/dom/element/scroll_view.h"
#include "runtime/dom/element/water_flow.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroDOM {

#define SET_PROPERTY_IF_NEED(PROPERTY_TYPE, PROPERTY_NAME)              \
    if (style_ref_->PROPERTY_NAME.has_value())                          \
        TaroCSSOM::TaroStylesheet::PROPERTY_TYPE::staticSetValueToNode( \
            ark_node_, style_ref_->PROPERTY_NAME.value());

    using ChangeType = TaroChange;

    void addDelayedCall(OperationType opType, std::shared_ptr<TaroRenderNode> parent, std::shared_ptr<TaroRenderNode> child, int index) {
        switch (opType) {
            case OperationType::AddChild: {
                auto task = [child, parent, index]() {
                    if (child->parent_ref_.lock() &&
                        child->GetArkUINodeHandle()) {
                        NativeNodeApi::getInstance()->addChild(
                            parent->GetArkUINodeHandle(),
                            child->GetArkUINodeHandle());
                        if (auto element = child->element_ref_.lock()) {
                            TARO_LOG_DEBUG("DirtyTask", "appendChild %{public}s %{public}d", element->class_name_.c_str(), index);
                        }
                    } else {
                        TARO_LOG_DEBUG("Render", "addDelayedCall Error AddChild ");
                    }
                };
                DirtyTaskPipeline::GetInstance()->AddUpdateTask(task);
            } break;
            case OperationType::InsertChildAt: {
                auto task = [child, parent, index]() {
                    if (child->parent_ref_.lock()) {
                        NativeNodeApi::getInstance()->insertChildAt(
                            parent->GetArkUINodeHandle(),
                            child->GetArkUINodeHandle(), index);
                        if (auto element = child->element_ref_.lock()) {
                            TARO_LOG_DEBUG("DirtyTask", "insertChild %{public}s %{public}d", element->class_name_.c_str(), index);
                        }
                    } else {
                        TARO_LOG_DEBUG("Render", "addDelayedCall Error InsertChildAt");
                    }
                };
                DirtyTaskPipeline::GetInstance()->AddUpdateTask(task);
            } break;
            case OperationType::RemoveChild: {
                auto task = [child, parent, index]() {
                    if (parent && child) {
                        NativeNodeApi::getInstance()->removeChild(parent->GetArkUINodeHandle(), child->GetArkUINodeHandle());
                        if (auto element = child->element_ref_.lock()) {
                            TARO_LOG_DEBUG("DirtyTask", "removeChild %{public}s %{public}d", element->class_name_.c_str(), index);
                        }
                    } else {
                        TARO_LOG_DEBUG("Render", "addDelayedCall Error RemoveChild ");
                    }
                };
                DirtyTaskPipeline::GetInstance()->AddUpdateTask(task);
            } break;
        }
    }

    int32_t TaroRenderNode::uid_flag_ = 1000;
    std::unordered_map<int32_t, std::weak_ptr<TaroRenderNode>> TaroRenderNode::custom_layout_render_nodes_;

    TaroRenderNode::TaroRenderNode() {}

    TaroRenderNode::TaroRenderNode(TaroDOM::TaroElementRef element)
        : BaseRenderNode(),
          element_ref_(element) {
        uid_ = uid_flag_++;
#if IS_DEBUG
        auto el = element_ref_.lock();
        element_nid_ = el->nid_;
        element_node_name_ = el->GetNodeName();
        element_class_name_ = el->class_name_;
#endif
    }

    TaroRenderNode::~TaroRenderNode() {
        animation_ = nullptr;
        style_ref_ = nullptr;
        old_style_ref_ = nullptr;

        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        if (is_custom_layout_) {
            nativeNodeApi->removeNodeCustomEventReceiver(ark_node_, OnStaticCustomEvent);
            custom_layout_render_nodes_.erase(uid_);
            nativeNodeApi->unregisterNodeCustomEvent(ark_node_, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE);
            nativeNodeApi->unregisterNodeCustomEvent(ark_node_, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT);
        }

        children_refs_.clear(); // Note: 确保子节点在 ark_node_ 释放前全部销毁
        if (ark_node_ != nullptr) {
            NativeNodeApi::getInstance()->disposeNode(ark_node_);
            ark_node_ = nullptr;
        }
#if IS_DEBUG
        TARO_LOG_DEBUG("~TaroRenderNode", "析构 %{public}d %{public}s", element_nid_, element_node_name_.c_str());
#endif
    }

    void TaroRenderNode::SetStyle(StylesheetRef style) {
        if (style->transition.has_value()) {
            TARO_LOG_DEBUG("TaroRenderNode", "transition render:%{public}p", this);
        }

        old_style_ref_ = style_ref_;
        style_ref_ = style;

        DimensionContext::UpdateCurrentContext(GetDimensionContext());

        // 清理Transition和Animation动画
        if (animation_ != nullptr) {
            animation_->clearStyleCSSAnimations();
        }

        const auto parent = parent_ref_.lock();
        SetNodeLayoutStyle(style_ref_, old_style_ref_, parent ? parent->style_ref_ : nullptr);
        SetNodeDrawStyle(style_ref_, old_style_ref_);
        SetFontDrawStyle(style_ref_, old_style_ref_);

        // 设置animation初始值
        if (style_ref_->animationMulti.has_value()) {
            if (animation_ == nullptr) {
                auto elem = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
                animation_ = std::make_shared<TaroAnimate::TaroNodeAnimations>(elem);
            }
            TARO_LOG_DEBUG("TaroRenderNode", "TaroRenderNode::SetStyle zero:%{public}p", this);
            animation_->setCSSAnimationZeroValue();
        }
    }

    float TaroRenderNode::GetComputedStyle(const char* name) const {
        return layoutDiffer_.GetComputedStyle(name);
    }

    void TaroRenderNode::SetCustomLayout() {
        is_custom_layout_ = true;
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        // 注册自定义事件监听器。
        nativeNodeApi->addNodeCustomEventReceiver(ark_node_, OnStaticCustomEvent);
        custom_layout_render_nodes_[uid_] = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        // 声明自定义事件并转递自身作为自定义数据。
        nativeNodeApi->registerNodeCustomEvent(ark_node_, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE, uid_, nullptr);
        nativeNodeApi->registerNodeCustomEvent(ark_node_, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT, uid_, nullptr);
    }

    void TaroRenderNode::OnDisplayChange(const PropertyType::Display& val, const PropertyType::Display& oldVal) {
        for (auto& child : children_refs_) {
            child->OnDisplayChange(val, oldVal);
        }
    }

    void TaroRenderNode::OnStaticCustomEvent(ArkUI_NodeCustomEvent* event) {
        // 获取组件实例对象，调用相关实例方法。
        int32_t uid = OH_ArkUI_NodeCustomEvent_GetEventTargetId(event);
        auto it = custom_layout_render_nodes_.find(uid);
        if (it != custom_layout_render_nodes_.end()) {
            if (auto customNode = it->second.lock()) {
                auto type = OH_ArkUI_NodeCustomEvent_GetEventType(event);
                switch (type) {
                    case ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE:
                        customNode->OnMeasure(event);
                        break;
                    case ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT:
                        customNode->OnLayout(event);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void TaroRenderNode::OnMeasure(ArkUI_NodeCustomEvent* event) {
        SystraceSection s("Taro Custom Layout:: OnMeasure");
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        // 创建子节点布局限制，复用父组件布局中的百分比参考值。
        auto childLayoutConstrain = OH_ArkUI_LayoutConstraint_Create();
        OH_ArkUI_LayoutConstraint_SetMaxHeight(childLayoutConstrain, static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.height)));
        OH_ArkUI_LayoutConstraint_SetMaxWidth(childLayoutConstrain, static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.width)));
        OH_ArkUI_LayoutConstraint_SetMinHeight(childLayoutConstrain, 0);
        OH_ArkUI_LayoutConstraint_SetMinWidth(childLayoutConstrain, 0);
        // 测算子节点获取子节点最大值。
        auto totalSize = nativeNodeApi->getTotalChildCount(ark_node_);
        for (uint32_t i = 0; i < totalSize; i++) {
            // auto child = nativeNodeApi->getChildAt(ark_node_, i); // Note: 当前部分组件会返回空指针，暂时不使用该方法
            auto child = children_refs_[i]->GetArkUINodeHandle();
            // 调用测算接口测算子组件。
            nativeNodeApi->measureNode(child, childLayoutConstrain);
        }

        nativeNodeApi->setMeasuredSize(
            ark_node_,
            static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.width)),
            static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.height)));
    }

    void TaroRenderNode::OnLayout(ArkUI_NodeCustomEvent* event) {
        SystraceSection s("Taro Custom Layout:: OnLayout");
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        // 获取父组件期望位置并设置。
        nativeNodeApi->setLayoutPosition(
            ark_node_,
            static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.left)),
            static_cast<int32_t>(vp2Px(layoutDiffer_.computed_style_.top)));
        for (auto& child : children_refs_) {
            nativeNodeApi->layoutNode(
                child->ark_node_,
                static_cast<int32_t>(vp2Px(child->layoutDiffer_.computed_style_.left)),
                static_cast<int32_t>(vp2Px(child->layoutDiffer_.computed_style_.top)));
        }
    }

    // anonymous namespace
    namespace {
        void ApplyLayout(std::shared_ptr<TaroRenderNode> node, bool layoutWithoutDiff) {
            if (!YGNodeGetHasNewLayout(node->ygNodeRef)) {
                node->SetLayoutDirty(false);
                return;
            }
            // Reset the flag
            YGNodeSetHasNewLayout(node->ygNodeRef, false);
            // Do the real work
            if (layoutWithoutDiff) {
                node->is_first_layout_finish_ = false;
            }
            node->Layout();
            // 启动Animation动画
            if (node->animation_ != nullptr) {
                node->animation_->startCSSAnimation();
            }

            if (node->layoutDiffer_.GetDisplayStyle() == YGDisplayNone) {
                // 如果display是none，没必要往下继续布局了，等它切回来的时候再布局
                return;
            }

            std::for_each(
                node->children_refs_.begin(), node->children_refs_.end(),
                [&](std::shared_ptr<TaroRenderNode> child) {
                    ApplyLayout(child, layoutWithoutDiff);
                });
        }
    }; // namespace

    void TaroRenderNode::LayoutAll(bool layoutWithoutDiff) {
        DimensionContext::UpdateCurrentContext(GetDimensionContext());
        ApplyLayout(std::static_pointer_cast<TaroRenderNode>(shared_from_this()), layoutWithoutDiff);
    }

    float calculateActualValue(YGNodeRef node, YGNodeRef parentNode, YGEdge edge) {
        YGValue position = YGNodeStyleGetPosition(node, edge);
        if (position.unit == YGUnitPercent) {
            float parentSize;
            if (edge == YGEdgeLeft || edge == YGEdgeRight) {
                parentSize = YGNodeLayoutGetWidth(parentNode);
            } else {
                parentSize = YGNodeLayoutGetHeight(parentNode);
            }
            return (position.value / 100.0f) * parentSize;
        }
        return position.value; // 如果不是百分比，直接返回值
    }

    // 处理脱离文档流：绝对定位的排版，需要特殊处理它的可用宽高
    void calculateAbsolutePositionedLayout(std::shared_ptr<TaroRenderNode> renderNode, YGNodeRef parentNode) {
        YGNodeRef node = renderNode->ygNodeRef;
        float parentWidth = YGNodeLayoutGetWidth(parentNode);
        float parentHeight = YGNodeLayoutGetHeight(parentNode);

        float availableWidth = parentWidth;
        float availableHeight = parentHeight;

        // 检查是否设置了具体的宽度
        if (!isnan(YGNodeStyleGetWidth(node).value)) {
            if (YGNodeStyleGetWidth(node).unit == YGUnitPercent) {
                availableWidth = (YGNodeStyleGetWidth(node).value / 100.0f) * parentWidth;
            } else {
                availableWidth = YGNodeStyleGetWidth(node).value;
            }
        }
        // 如果没有设置宽度，但设置了 left 和 right，计算可用宽度
        else if (YGNodeStyleGetPositionType(node) == YGPositionType::YGPositionTypeAbsolute && !isnan(YGNodeStyleGetPosition(node, YGEdgeLeft).value) && !isnan(YGNodeStyleGetPosition(node, YGEdgeRight).value)) {
            float leftValue = calculateActualValue(node, parentNode, YGEdgeLeft);
            float rightValue = calculateActualValue(node, parentNode, YGEdgeRight);
            availableWidth = parentWidth - leftValue - rightValue;
        } else if (!renderNode->style_ref_->width.has_value()) {
            availableHeight = YGUndefined;
            parentWidth = 0; // 宽度也没设置，不约束，由子元素撑开
        }

        // 对高度进行类似的处理
        if (!isnan(YGNodeStyleGetHeight(node).value)) {
            if (YGNodeStyleGetHeight(node).unit == YGUnitPercent) {
                availableHeight = (YGNodeStyleGetHeight(node).value / 100.0f) * parentHeight;
            } else {
                availableHeight = YGNodeStyleGetHeight(node).value;
            }
        } else if (YGNodeStyleGetPositionType(node) == YGPositionType::YGPositionTypeAbsolute && !isnan(YGNodeStyleGetPosition(node, YGEdgeTop).value) && !isnan(YGNodeStyleGetPosition(node, YGEdgeBottom).value)) {
            float topValue = calculateActualValue(node, parentNode, YGEdgeTop);
            float bottomValue = calculateActualValue(node, parentNode, YGEdgeBottom);
            availableHeight = parentHeight - topValue - bottomValue;
        } else if (!renderNode->style_ref_->height.has_value()) {
            availableHeight = YGUndefined;
            parentHeight = 0; // 高度也没设置，不约束，由子元素撑开
        }

        if (parentHeight != 0) {
            availableHeight = availableHeight > parentHeight ? availableHeight : parentHeight;
            if (availableHeight == 0) {
                availableHeight = YGUndefined;
            }
        }
        if (parentWidth != 0) {
            availableWidth = availableWidth > parentWidth ? availableWidth : parentWidth;
            if (availableWidth == 0) {
                availableWidth = YGUndefined;
            }
        }
        TARO_LOG_DEBUG("TaroRenderNode", "%{public}f, %{public}f, %{public}f, %{public}f", availableWidth, availableHeight, parentWidth, parentHeight);

        // 调用布局计算
        YGNodeCalculateLayout(node, availableWidth, availableHeight, YGNodeStyleGetDirection(parentNode));
    }

    void TaroRenderNode::MeasureLocal() {
        bool measureSelf = IsDetachFromDocumentFlow();
        auto element = element_ref_.lock();

        if (measureSelf) {
            // 绝对定位节点和 lazyload 节点重新排版，不影响兄弟节点
            Measure();
        } else {
            if (auto parent = parent_ref_.lock()) {
                // 父节点重新排版
                parent->Measure();
                YGNodeSetHasNewLayout(parent->ygNodeRef, false);
                // 让节点以及节点上级的flag清空掉，防止下次布局的时候再次进行布局
                // auto currentParent = parent;
                // while (currentParent) {
                //     YGNodeSetHasNewLayout(currentParent->ygNodeRef, false);
                //     currentParent = currentParent->parent_ref_.lock();
                // }
            }
        }
    }

    void TaroRenderNode::LayoutLocal() {
        DimensionContext::UpdateCurrentContext(GetDimensionContext());
        bool measureSelf = IsDetachFromDocumentFlow();
        auto element = element_ref_.lock();
        bool isLazyChild = element && element->GetParentNode()->lazy_node;

        if (measureSelf || isLazyChild) {
            // 绝对定位节点和 lazyload 节点重新布局，不影响兄弟节点
            LayoutAll();
        } else {
            if (auto parent = parent_ref_.lock()) {
                // 兄弟节点与自己进行重新布局
                for (auto child : parent->children_refs_) {
                    child->LayoutAll();
                }
            }
        }
    }

    TaroCSSOM::TaroStylesheet::BackgroundSizeParam TaroRenderNode::CalcBackgroundSize(Optional<TaroCSSOM::TaroStylesheet::BackgroundImageItem> backgroundImage) {
        float imgWidth = NAN; // 图片的宽高
        float imgHeight = NAN;

        float actualWidth = NAN; // 最终计算出来的宽高
        float actualHeight = NAN;
        // 获取 background图片的原始宽高

        if (backgroundImage.has_value() && backgroundImage.value().type == TaroCSSOM::TaroStylesheet::PIC) {
            if (auto imageInfo = std::get_if<TaroHelper::ResultImageInfo>(&(backgroundImage.value().src))) {
                imgWidth = imageInfo->width;
                imgHeight = imageInfo->height;
            }
        }
        /**
         * 根据 background-size的设置算出最终图片的宽高
         */
        auto backgroundSize = paintDiffer_.paint_style_->background_size_.value;
        if (!backgroundSize.has_value()) { // 如果background-size 没设，图片的实际宽高就是最终宽高
            actualWidth = imgWidth;
            actualHeight = imgHeight;
        } else if (backgroundSize.value().type == TaroCSSOM::TaroStylesheet::SIZEOPTIONS) { // 设置图片宽高
            float sizeWidth = NAN;
            float sizeHeight = NAN;

            // 拿 size 设置的宽高
            if (backgroundSize.value().width.has_value()) {
                if (auto valOptional = backgroundSize.value().width.value().ParseToVp(GetDimensionContext(), layoutDiffer_.computed_style_.width); valOptional.has_value()) {
                    sizeWidth = valOptional.value();
                }
            }

            if (backgroundSize.value().height.has_value()) {
                if (auto valOptional = backgroundSize.value().height.value().ParseToVp(GetDimensionContext(), layoutDiffer_.computed_style_.height); valOptional.has_value()) {
                    sizeHeight = valOptional.value();
                }
            }

            if (std::isnan(sizeWidth) && std::isnan(sizeHeight)) { // 如果两个都没设, 设图片宽高
                actualWidth = imgWidth;
                actualHeight = imgHeight;
            } else if (!std::isnan(sizeWidth) && !std::isnan(sizeHeight)) { // 两个都有，设这两个
                actualWidth = sizeWidth;
                actualHeight = sizeHeight;
            } else { // 其中一个是nan的话,根据有的把没有的算出来
                if (std::isnan(sizeWidth)) {
                    actualHeight = sizeHeight;
                    actualWidth = (actualHeight / imgHeight) * imgWidth;
                }
                if (std::isnan(sizeHeight)) {
                    actualWidth = sizeWidth;
                    actualHeight = (actualWidth / imgWidth) * imgHeight;
                }
            }

        } else if (backgroundSize.value().type == TaroCSSOM::TaroStylesheet::IMAGESIZE) { // 设置 cover 或者 contain
            float imageAspectRatio = imgWidth / imgHeight;
            float containerAspectRatio = layoutDiffer_.computed_style_.width / layoutDiffer_.computed_style_.height;
            if (backgroundSize.value().imageSize == ARKUI_IMAGE_SIZE_CONTAIN) { // 图片的一条边跟容器一样，另一条小于容器

                if (imageAspectRatio > containerAspectRatio) { // 图片的宽高比比容器大，以容器的宽进行缩放
                    actualWidth = layoutDiffer_.computed_style_.width;
                    actualHeight = layoutDiffer_.computed_style_.width / imageAspectRatio;
                } else if (imageAspectRatio <= containerAspectRatio) { // 图片的宽高比比容器小，以容器的高度进行缩放
                    actualWidth = layoutDiffer_.computed_style_.height * imageAspectRatio;
                    actualHeight = layoutDiffer_.computed_style_.height;
                } else {
                    // 一些异常情况，不用处理
                }
            } else if (backgroundSize.value().imageSize == ARKUI_IMAGE_SIZE_COVER) { // 图片的一条边跟容器一样，另一条大于容器

                if (imageAspectRatio > containerAspectRatio) { // 图片的宽高比比容器大， 以容器的高度进行缩放
                    actualWidth = layoutDiffer_.computed_style_.height * imageAspectRatio;
                    actualHeight = layoutDiffer_.computed_style_.height;

                } else if (imageAspectRatio <= containerAspectRatio) { // 图片的宽高比比容器小，以容器的宽进行缩放
                    actualWidth = layoutDiffer_.computed_style_.width;
                    actualHeight = layoutDiffer_.computed_style_.width / imageAspectRatio;
                } else {
                    // 一些异常情况，不用处理
                }
            } else if (backgroundSize.value().imageSize == ARKUI_IMAGE_SIZE_AUTO) {
                actualWidth = imgWidth;
                actualHeight = imgHeight;
            }
        }

        TaroCSSOM::TaroStylesheet::BackgroundSizeParam backgroundSizeParam;
        backgroundSizeParam.type = TaroCSSOM::TaroStylesheet::SIZEOPTIONS;
        if (!std::isnan(actualWidth)) {
            backgroundSizeParam.width = Dimension{actualWidth, DimensionUnit::VP};
        }
        if (!std::isnan(actualHeight)) {
            backgroundSizeParam.height = Dimension{actualHeight, DimensionUnit::VP};
        }
        return backgroundSizeParam;
    }

    void TaroRenderNode::MeasureAndLayoutLocal() {
        MeasureLocal();
        LayoutLocal();
    }

    void TaroRenderNode::Measure() {
        SystraceSection s("Taro Measure");
        TIMER_US_FUNCTION();

        auto item = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        if (item && item->ygNodeRef) {
            auto parentYgRef = YGNodeGetParent(item->ygNodeRef);
            if (!parentYgRef) {
                auto dimensionContext = GetDimensionContext();
                // 根节点，约束宽度
                // 在 dirtyTask 中进行 root Measure 的情况会调用
                if (auto element = element_ref_.lock()) {
                    TARO_LOG_DEBUG("DirtyTask vsync", "Measure root %{public}s, width: %{public}f, height: %{public}f", element->class_name_.c_str(), dimensionContext->viewport_width_, dimensionContext->viewport_height_);
                }
                TaroYogaApi::getInstance()->calcYGLayout(item->ygNodeRef, dimensionContext->viewport_width_, dimensionContext->viewport_height_);
            } else {
                // 这种情况目前只有 calc 的情况下会调用
                if (IsDetachFromDocumentFlow()) {
                    // 脱离文档流，需要特殊处理
                    calculateAbsolutePositionedLayout(item, parentYgRef);
                }

                float constraintHeight = YGNodeLayoutGetHeight(item->ygNodeRef);
                float constraintWidth = YGNodeLayoutGetWidth(item->ygNodeRef);

                float availableWidth = TaroGetMeasureAvailableWidth(item->ygNodeRef);
                float availableHeight = TaroGetMeasureAvailableHeight(item->ygNodeRef);
                YGMeasureMode widthMode = TaroGetWidthMeasureMode(item->ygNodeRef);
                YGMeasureMode heightMode = TaroGetHeightMeasureMode(item->ygNodeRef);

                switch (widthMode) {
                    case YGMeasureModeAtMost: {
                        constraintWidth = std::min(constraintWidth, availableWidth);
                        break;
                    }
                    case YGMeasureModeUndefined:
                        constraintWidth = YGUndefined;
                        break;
                    case YGMeasureModeExactly:
                        constraintWidth = availableWidth;
                        break;
                }
                switch (heightMode) {
                    case YGMeasureModeAtMost: {
                        constraintHeight = std::min(constraintHeight, availableHeight);
                        break;
                    }
                    case YGMeasureModeUndefined:
                        constraintHeight = YGUndefined;
                        break;
                    case YGMeasureModeExactly:
                        constraintHeight = availableHeight;
                        break;
                }

                if (auto parentNodeRef = parent_ref_.lock()) {
                    // ScrollWrapper的高度/宽度不需要约束
                    auto scroll = std::dynamic_pointer_cast<TaroScrollNode>(parentNodeRef);
                    if (scroll) {
                        if (scroll->direction_ == ARKUI_SCROLL_DIRECTION_HORIZONTAL) {
                            constraintWidth = YGUndefined;
                        } else {
                            constraintHeight = YGUndefined;
                        }
                    }

                    if (auto water_flow = std::dynamic_pointer_cast<TaroWaterFlowNode>(parentNodeRef)) {
                        constraintHeight = YGUndefined;
                    } else if (auto list = std::dynamic_pointer_cast<TaroListNode>(parentNodeRef)) {
                        constraintHeight = YGUndefined;
                    }
                }

                if (auto element = element_ref_.lock()) {
                    TARO_LOG_DEBUG("DirtyTask vsync", "Measure %{public}s, width: %{public}f, height: %{public}f, availableWidth: %{public}f, widthMode: %{public}d, availableHeight: %{public}f, heightMode: %{public}d", element->class_name_.c_str(), constraintWidth, constraintHeight, availableWidth, widthMode, availableHeight, heightMode);
                }
                TaroYogaApi::getInstance()->calcYGLayout(item->ygNodeRef, constraintWidth, constraintHeight);
            }
        }
    }

    void TaroRenderNode::Layout() {
        SystraceSection s("Taro Layout::Layout Start");
        if (is_layout_dirty_) {
            SetLayoutDirty(false);
        };

        layoutDiffer_.SetComputedStyle(ygNodeRef, !is_apply_reused);
        is_apply_reused = false;

        if (auto element = element_ref_.lock()) {
            TARO_LOG_DEBUG("dirty", "ReLayout className: %{public}s, width: %{public}f, height: %{public}f, left: %{public}f, top: %{public}f",
                           element->class_name_.c_str(),
                           layoutDiffer_.computed_style_.width,
                           layoutDiffer_.computed_style_.height,
                           layoutDiffer_.computed_style_.left,
                           layoutDiffer_.computed_style_.top);
        }

        LayoutDiffer::DiffAndSetStyle(layoutDiffer_.computed_style_, layoutDiffer_.old_computed_style_, this);

        // 布局改变后，需要对部分影响到的绘制样式重新标脏>绘制
        MakeDrawPropertyDirtyFromLayoutEffect();
        if (!is_first_layout_finish_) {
            is_first_layout_finish_ = true;
        }
    }

    void TaroRenderNode::Paint() {
        DimensionContext::UpdateCurrentContext(GetDimensionContext());
        if (!style_ref_) {
            TARO_LOG_DEBUG("TaroRenderNode", "No Style Paint");
            return;
        }
#if IS_DEBUG
        TARO_LOG_DEBUG("dirty", "Paint %{public}s", element_class_name_.c_str());
#endif
        if (is_draw_dirty_) {
            SetDrawDirty(false);
        }
        paintDiffer_.DiffAndSetStyle(*paintDiffer_.paint_style_, *paintDiffer_.old_paint_style_, this);
    }

    void TaroRenderNode::ForceUpdate() {
        SetDrawDirty(true);
        SetLayoutDirty(true);
    }

    void TaroRenderNode::ClearDifferOldStyleFromElement() {
        paintDiffer_.ClearOldStyle();
        layoutDiffer_.old_computed_style_ = ComputedStyle();
        is_apply_reused = true;
        ForceUpdate();
    }

    void TaroRenderNode::UpdateDifferOldStyleFromElement(std::weak_ptr<TaroDOM::TaroElement> element_ref) {
        if (auto reuse_element = element_ref.lock()) {
            if (auto reuse_render_node = reuse_element->GetHeadRenderNode()) {
                *paintDiffer_.old_paint_style_ = *reuse_render_node->paintDiffer_.paint_style_;
                layoutDiffer_.old_computed_style_ = reuse_render_node->layoutDiffer_.computed_style_;
                is_apply_reused = true;
                ForceUpdate();
            }
        }
    }

    void TaroRenderNode::setBackgroundImageAndPositionAndSize() {
        // 没有背景图片，一切免谈
        if (!paintDiffer_.paint_style_->background_image_.value.has_value()) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundImage(ark_node_, TaroCSSOM::TaroStylesheet::BackgroundImageItem::emptyImg);
            return;
        }
        auto bgImg = paintDiffer_.paint_style_->background_image_.value.value();
        if (auto url = std::get_if<std::string>(&bgImg.src); url && bgImg.type == TaroCSSOM::TaroStylesheet::PIC) {
            if (!url->empty()) {
                if (url->starts_with("//")) {
                    url->insert(0, "https:");
                }
                std::weak_ptr<TaroRenderNode> weakRenderNode = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
                auto oldUrl = *url;
                TaroHelper::loadImage({.url = *url}, [weakRenderNode, oldUrl](
                        const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>& result) {
                    if (auto self = weakRenderNode.lock()) {
                        // 拿到执行的时候的当前background url跟捕获到URL 是否一样，不一样就不设了
                        if (self->paintDiffer_.paint_style_->background_image_.value.has_value()) {
                            auto bgImg = self->paintDiffer_.paint_style_->background_image_.value.value();
                            if (auto currentUrl = std::get_if<std::string>(&bgImg.src); currentUrl && bgImg.type == TaroCSSOM::TaroStylesheet::PIC) {
                                if (oldUrl.find(*currentUrl) != std::string::npos) {
                                    self->HandleBgImageLoad(result, oldUrl);
                                }
                            }
                        }
                    }
                });
            }
        } else {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundImage(ark_node_, paintDiffer_.paint_style_->background_image_.value, paintDiffer_.paint_style_->background_repeat_.value);
        }
    }

    void TaroRenderNode::HandleBgImageLoad(const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>& result, const std::string url) {
        TaroCSSOM::TaroStylesheet::BackgroundImageItem b;
        b.type = TaroCSSOM::TaroStylesheet::PIC;
        if (auto res = std::get_if<TaroHelper::ErrorImageInfo>(&result); res) {
            if (res->isUnsupportedType) {
                b.src = res->url;
            } else {
                b = TaroCSSOM::TaroStylesheet::BackgroundImageItem::emptyImg;
            }
        } else if (auto res = std::get_if<TaroHelper::ResultImageInfo>(&result); res) {
            b.src = *res;
            auto backgroundSizeItem = CalcBackgroundSize(b);
            if (paintDiffer_.paint_style_->background_position_.value[0].has_value() || paintDiffer_.paint_style_->background_position_.value[1].has_value()) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundPosition(
                    ark_node_, paintDiffer_.paint_style_->background_position_.value[0], paintDiffer_.paint_style_->background_position_.value[1],
                    layoutDiffer_.computed_style_.width, layoutDiffer_.computed_style_.height, backgroundSizeItem);
            }

            if (paintDiffer_.paint_style_->background_size_.value.has_value()) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundSize(ark_node_, backgroundSizeItem, layoutDiffer_.computed_style_.width, layoutDiffer_.computed_style_.height);
            }
            relatedImageDrawableDescriptors.push_back(res->result_DrawableDescriptor);
        } else {
            // 兜底状态，一般不会触发
            b.src = url;
        }
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundImage(ark_node_, b, paintDiffer_.paint_style_->background_repeat_.value);
    }

    bool TaroRenderNode::GetIsInline() {
        return isInline_;
    }

    void TaroRenderNode::SetIsInline(bool isInline) {
        isInline_ = isInline;
    }

    // 判断是否脱离文档流
    bool TaroRenderNode::IsDetachFromDocumentFlow() {
        if (style_ref_ && style_ref_->position.has_value()) {
            auto position = style_ref_->position.value();
            return position == PropertyType::Position::Absolute || position == PropertyType::Position::Fixed;
        }
        return false;
    }

    ArkUI_NodeHandle TaroRenderNode::GetArkUINodeHandle() {
        return ark_node_;
    }

    void TaroRenderNode::SetArkUINodeHandle(ArkUI_NodeHandle handle) {
        ark_node_ = handle;
    }

    std::shared_ptr<TaroRenderNode> TaroRenderNode::GetRootRenderNode() {
        auto element = element_ref_.lock();
        if (element) {
            const auto pageRoot = element->getPageRoot();
            if (pageRoot) {
                return pageRoot->GetHeadRenderNode();
            }
        }
        return nullptr;
    }

    void TaroRenderNode::AppendChild(const std::shared_ptr<TaroRenderNode>& child) {
        if (!child)
            return;
        child->parent_ref_ = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        children_refs_.push_back(child);
        int32_t index = children_refs_.size() - 1;
        TaroYogaApi::getInstance()->insertChildToParent(
            ygNodeRef, child->ygNodeRef, index);

        if (IfImmediateAttach(child)) {
            NativeNodeApi::getInstance()->addChild(GetArkUINodeHandle(), child->GetArkUINodeHandle());
        } else {
            addDelayedCall(
                OperationType::AddChild, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), child, index);
        }
    }

    void TaroRenderNode::RemoveChild(const std::shared_ptr<TaroRenderNode>& child) {
        if (child == nullptr || this == nullptr)
            return;
        // 处理flowsection， flowsection的rendernode是父节点waterflow的rendernode
        if (this == child.get()) {
            return;
        }

        if (child->animation_ != nullptr) {
            TARO_LOG_DEBUG("TaroRenderNode", "RemoveChild animation");
            child->animation_ = nullptr;
        }

        int32_t index = 0;
        auto it = std::find(children_refs_.begin(), children_refs_.end(), child);
        if (it == children_refs_.end()) {
            // Fixed场景可能不一定挂载parent上，会挂到root上，所以它的parent可能是root的fix_root
            const auto parent_render_node = child->parent_ref_.lock();
            if (parent_render_node) {
                parent_render_node->RemoveChild(child);
            }
            return;
        } else {
            index = std::distance(children_refs_.begin(), it);
        }

        child->parent_ref_.reset();
        child->ResetFixedVisibility();
        children_refs_.erase(it);

        TaroYogaApi::getInstance()->removeChild(ygNodeRef, child->ygNodeRef);

        if (IfImmediateAttach(child)) {
            NativeNodeApi::getInstance()->removeChild(GetArkUINodeHandle(), child->GetArkUINodeHandle());
        } else {
            addDelayedCall(OperationType::RemoveChild, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), child, index);
        }
    }

    void TaroRenderNode::ReplaceChild(
        const std::shared_ptr<TaroRenderNode>& old_child,
        const std::shared_ptr<TaroRenderNode>& new_child) {
        auto it =
            std::find(children_refs_.begin(), children_refs_.end(), old_child);
        if (it != children_refs_.end()) {
            bool is_new_child_attach_immediate = IfImmediateAttach(new_child);
            if (is_new_child_attach_immediate) {
                if (auto old_element = old_child->element_ref_.lock()) {
                    old_element->SetStateFlag(TaroRuntime::STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
                }
            }

            RemoveChild(old_child);
            if (IfImmediateAttach(old_child)) {
                if (auto old_element = old_child->element_ref_.lock()) {
                    old_element->ClearStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
                }
            }
            auto index = std::distance(children_refs_.begin(), it);
            new_child->parent_ref_ = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            children_refs_.insert(it, new_child);
            TaroYogaApi::getInstance()->insertChildToParent(
                ygNodeRef, new_child->ygNodeRef,
                std::distance(children_refs_.begin(), it));

            if (is_new_child_attach_immediate) {
                NativeNodeApi::getInstance()->insertChildAt(
                    GetArkUINodeHandle(), new_child->GetArkUINodeHandle(),
                    index);
            } else {
                addDelayedCall(
                    OperationType::InsertChildAt, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), new_child,
                    index);
            }
        }
    }

    void TaroRenderNode::InsertChildAt(
        const std::shared_ptr<TaroRenderNode>& child, uint8_t index) {
        child->parent_ref_ = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        children_refs_.insert(children_refs_.begin() + index, child);
        TaroYogaApi::getInstance()->insertChildToParent(
            ygNodeRef, child->ygNodeRef, index);

        if (IfImmediateAttach(child)) {
            NativeNodeApi::getInstance()->insertChildAt(
                GetArkUINodeHandle(), child->GetArkUINodeHandle(), index);
        } else {
            addDelayedCall(
                OperationType::InsertChildAt, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), child, index);
        }
    }

    void TaroRenderNode::UpdateChild(const std::shared_ptr<TaroRenderNode>& child) {
        auto it = std::find(children_refs_.begin(), children_refs_.end(), child);
        if (it == children_refs_.end()) {
            return;
        }
        auto index = std::distance(children_refs_.begin(), it);
        NativeNodeApi::getInstance()->insertChildAt(
            GetArkUINodeHandle(), child->GetArkUINodeHandle(), index);
    }

    void TaroRenderNode::InsertChildBefore(
        const std::shared_ptr<TaroRenderNode>& child,
        const std::shared_ptr<TaroRenderNode>& sibling) {
        if (child == nullptr || sibling == nullptr)
            return;

        auto it = std::find(children_refs_.begin(), children_refs_.end(), sibling);
        if (it != children_refs_.end()) {
            child->parent_ref_ = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            auto index = std::distance(children_refs_.begin(), it);
            children_refs_.insert(it, child);
            TaroYogaApi::getInstance()->insertChildToParent(ygNodeRef, child->ygNodeRef, index);
            if (IfImmediateAttach(child)) {
                NativeNodeApi::getInstance()->insertChildAt(
                    GetArkUINodeHandle(), child->GetArkUINodeHandle(), index);
            } else {
                addDelayedCall(
                    OperationType::InsertChildAt, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), child,
                    index);
            }
        }
    }

    void TaroRenderNode::InsertChildAfter(
        const std::shared_ptr<TaroRenderNode>& child,
        const std::shared_ptr<TaroRenderNode>& sibling) {
        if (child == nullptr || sibling == nullptr)
            return;

        auto it = std::find(children_refs_.begin(), children_refs_.end(), sibling);
        if (it != children_refs_.end()) {
            child->parent_ref_ = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            auto index = std::distance(children_refs_.begin(), it);
            children_refs_.insert(std::next(it), child);
            TaroYogaApi::getInstance()->insertChildToParent(ygNodeRef, child->ygNodeRef, index + 1);
            if (IfImmediateAttach(child)) {
                NativeNodeApi::getInstance()->insertChildAt(
                    GetArkUINodeHandle(), child->GetArkUINodeHandle(),
                    index + 1);
            } else {
                addDelayedCall(
                    OperationType::InsertChildAt, std::static_pointer_cast<TaroRenderNode>(shared_from_this()), child,
                    index + 1);
            }
        }
    }

    void TaroRenderNode::SetShouldPosition(bool flag) {
        should_position_ = flag;
    }
    bool TaroRenderNode::GetShouldPosition() {
        return should_position_;
    }

    bool TaroRenderNode::IfImmediateAttach(const std::shared_ptr<TaroRenderNode>& child) {
        const auto elementRef = child->element_ref_.lock();
        if (elementRef) {
            if (elementRef->HasStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE)) {
                return true;
            }
        }
        return false;
    }

    bool TaroRenderNode::OnSetPropertyIntoNode(const CSSProperty::Type& property,
                                               const TaroChange& changeType,
                                               const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet>& style) {
        // todo: wzq Animation Forwards属性和进行中的Animation动画不设置
        if (animation_ == nullptr && style_ref_->transition.has_value()) {
            auto elem = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            animation_ = std::make_shared<TaroAnimate::TaroNodeAnimations>(elem);
        }

        bool anim_ret = true;
        if (animation_ != nullptr) {
            anim_ret = animation_->onSetPropertyIntoNode(property, changeType);
        }
        return anim_ret;
    }

    template <typename T, size_t size>
    bool ArrayPropertyHasSet(std::array<Optional<T>, size> property) {
        return std::any_of(property.begin(), property.end(), [](Optional<T> item) {
            return item.has_value();
        });
    }

    void TaroRenderNode::MakeDrawPropertyDirtyFromLayoutEffect() {
        bool shouldReDraw = false;
        auto paint_style = paintDiffer_.paint_style_;
        if (ArrayPropertyHasSet(paint_style->border_radius_.value)) {
            paint_style->border_radius_.force_update = true;
            shouldReDraw = true;
        }
        if (ArrayPropertyHasSet(paint_style->background_position_.value)) {
            paint_style->background_position_.force_update = true;
            shouldReDraw = true;
        }
        if (paint_style->background_size_.value.has_value()) {
            paint_style->background_size_.force_update = true;
            shouldReDraw = true;
        }
        if (paint_style->transform_.value.has_value()) {
            paint_style->transform_.force_update = true;
            shouldReDraw = true;
        }
        if (paint_style->transform_origin_.value.has_value()) {
            paint_style->transform_origin_.force_update = true;
            shouldReDraw = true;
        }
        if (shouldReDraw) {
            SetDrawDirty(true);
        }
    }

    int TaroRenderNode::createJsAnimation(TaroAnimate::TaroJsAnimationOptionRef& option) {
        if (this == nullptr) {
            return 0;
        }
        if (animation_ == nullptr) {
            auto elem = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            animation_ = std::make_shared<TaroAnimate::TaroNodeAnimations>(elem);
        }
        return animation_->bindJSAnimation(option);
    }

    void TaroRenderNode::AddNotifyFixedRoot() {
        if (auto elementRef = element_ref_.lock()) {
            if (auto pageRoot = elementRef->getPageRoot()) {
                DirtyTaskPipeline::GetInstance()->AddNotifyFixedRootNodes(pageRoot);
            }
        }
    }

    GlobalPostion TaroRenderNode::calcGlobalPostion() {
        GlobalPostion parent_position;
        auto render_node = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        auto element = element_ref_.lock();

        if (!element || !render_node) {
            return parent_position;
        }

        double position_x = 0.0f;
        double position_y = 0.0f;
        double offset_x = 0.0f;
        double offset_y = 0.0f;
        double scroll_top = 0.0f;
        double scroll_left = 0.0f;

        bool is_hidden = false;
        bool is_lazy_index_outside = false;

        if (element->style_->display.value_or(PropertyType::Display::Block) == PropertyType::Display::None) {
            is_hidden = true;
        }

        if (is_hidden) {
            parent_position.is_hidden = true;
            return parent_position;
        }

        auto parent_node = element->GetParentNode();

        is_lazy_index_outside = !element->checkIsInScrollContainerIndex();

        if (auto scroll_container_node = std::dynamic_pointer_cast<TaroScrollContainerNode>(render_node->parent_ref_.lock())) {
            auto scroll_container = std::static_pointer_cast<TaroScrollerContainer>(element);
            scroll_top = scroll_container->scrollTop_;
            scroll_left = scroll_container->scrollLeft_;
        }

        if (is_lazy_index_outside) {
            parent_position.is_lazy_index_outside = true;
            return parent_position;
        }

        ArkUI_IntOffset offset{0, 0};
        int32_t res = OH_ArkUI_NodeUtils_GetLayoutPosition(render_node->GetArkUINodeHandle(), &offset);
        if (res != 0) {
            TARO_LOG_ERROR("CalculateNodeVisibility", "GetLayoutPositionInWindow failed %{public}d", res);
        } else {
            position_x = TaroRuntime::px2Vp(offset.x);
            position_y = TaroRuntime::px2Vp(offset.y);
        }

        // 特殊处理 ListItemGroup 场景，因为 ListItem 在 ListItemGroup 是懒上屏的，但又没有可见性相关的 api
        if (std::dynamic_pointer_cast<TaroListItem>(element) && std::dynamic_pointer_cast<TaroListItemGroup>(parent_node)) {
            ArkUI_IntSize size{0, 0};
            res = OH_ArkUI_NodeUtils_GetLayoutSize(render_node->GetArkUINodeHandle(), &size);
            if (res != 0) {
                TARO_LOG_ERROR("CalculateNodeVisibility", "GetLayoutSize failed %{public}d", res);
            } else {
                // 大小和位置都为 0，证明为非法值，根据文档，该 item 不在 group 的可视范围内
                if (offset.x == 0 && offset.y == 0 && size.width == 0 && size.height == 0) {
                    parent_position.is_lazy_index_outside = true;
                    return parent_position;
                    // BUG 情况，位置为 0，但大小不为 0，但是前一个兄弟节点位置不为 0，鸿蒙大小取值有 bug
                } else if (offset.x == 0 && offset.y == 0) {
                    std::shared_ptr<TaroNode> prevNode = element->GetPreviousSibling();
                    auto prevElement = std::static_pointer_cast<TaroElement>(prevNode);
                    if (prevElement) {
                        auto prev_render_node = prevElement->GetHeadRenderNode();
                        if (prev_render_node) {
                            ArkUI_IntOffset prev_offset{0, 0};
                            res = OH_ArkUI_NodeUtils_GetLayoutPosition(prev_render_node->GetArkUINodeHandle(), &prev_offset);
                            if (res != 0) {
                                TARO_LOG_ERROR("CalculateNodeVisibility", "prevElement GetLayoutPositionInWindow failed %{public}d", res);
                            } else if (prev_offset.x != 0 || prev_offset.y != 0) {
                                parent_position.is_lazy_index_outside = true;
                                return parent_position;
                            }
                        }
                    }
                }
            }
        }

        // TARO_LOG_DEBUG("CalculateNodeVisibility", "%{public}s %{public}f %{public}d", element->class_name_.c_str(), position_y, offset.y);

        if (auto parent = parent_ref_.lock()) {
            parent_position = parent->calcGlobalPostion();
        }

        // 计算当前节点的全局位置
        return {
            parent_position.global_x + position_x,
            parent_position.global_y + position_y,
            parent_position.offset_x + offset_x,
            parent_position.offset_y + offset_y,
            parent_position.scroll_x + scroll_left,
            parent_position.scroll_y + scroll_top,
            parent_position.is_hidden,
            parent_position.is_lazy_index_outside};
    }
} // namespace TaroDOM
} // namespace TaroRuntime
