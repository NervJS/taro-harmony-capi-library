/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_visible.h"

#include "runtime/cssom/dimension/context.h"
#include "runtime/dom/element/element.h"

void VisibleEventListener::SetViewport(CallbackInfo& callback_info) {
    callback_info.viewport = {
        0,
        0,
    };
}

void VisibleEventListener::SetMargin(CallbackInfo& callback_info, ViewportMargin& margin) {
    callback_info.margin = margin;
}

int32_t VisibleEventListener::Register(std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>& element,
                                       ViewportMargin margin,
                                       std::unordered_map<float, bool> thresholds,
                                       float initialRatio, napi_ref callback_ref) {
    CallbackInfo callback_info;
    callback_info.triggeredThresholds_ = thresholds;
    callback_info.initialRatio = initialRatio;
    callback_info.callback_ref = callback_ref;

    SetMargin(callback_info, margin);
    SetViewport(callback_info);

    element->bindVisibleEvent(callback_info);

    return 0;
}

void VisibleEventListener::Disconnect(std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>& element) {
    element->unbindVisibleEvent();
}

void VisibleEventListener::updateVisibilityInfo(
    const CallbackInfo& callback_info,
    float node_left, float node_top,
    float node_width, float node_height,
    VisibilityInfo& visibilityInfo) {
    Viewport viewport = callback_info.viewport;
    ViewportMargin margin = callback_info.margin;

    float viewport_x = viewport.x - margin.left;
    float viewport_y = viewport.y - margin.top;
    float viewport_origin_width = TaroRuntime::DimensionContext::GetInstance()->viewport_width_;
    float viewport_origin_height = TaroRuntime::DimensionContext::GetInstance()->viewport_height_;
    float viewport_width = viewport_origin_width + margin.left + margin.right;
    float viewport_height = viewport_origin_height + margin.top + margin.bottom;
    // 判断节点是否在视窗内
    if (node_left + node_width < viewport_x ||
        node_left > viewport_x + viewport_width ||
        node_top + node_height < viewport_y ||
        node_top > viewport_y + viewport_height) {
        return; // 返回 intersectionRatio 为
                // -1.0，其他值为默认初始化状态
    }

    // 计算节点在视窗内露出的面积
    double visible_left = std::max(node_left, viewport_x);
    double visible_top = std::max(node_top, viewport_y);
    double visible_right =
        std::min(node_left + node_width, viewport_x + viewport_width);
    double visible_bottom =
        std::min(node_top + node_height, viewport_y + viewport_height);
    double visible_area =
        (visible_right - visible_left) * (visible_bottom - visible_top);

    // 计算节点总面积
    double total_area = node_width * node_height;

    // 计算露出的阈值，保留两位小数
    visibilityInfo.intersectionRatio =
        total_area > 0 ? std::round(visible_area / total_area * 100.0) / 100.0 : 0.0f;

    // 设置intersectionRect
    visibilityInfo.rect.intersectionRect.left = visible_left;
    visibilityInfo.rect.intersectionRect.right = visible_right;
    visibilityInfo.rect.intersectionRect.top = visible_top;
    visibilityInfo.rect.intersectionRect.bottom = visible_bottom;
    visibilityInfo.rect.intersectionRect.width = visible_right - visible_left;
    visibilityInfo.rect.intersectionRect.height = visible_bottom - visible_top;

    // 设置boundingClientRect
    visibilityInfo.rect.boundingClientRect.left = node_left;
    visibilityInfo.rect.boundingClientRect.right = node_left + node_width;
    visibilityInfo.rect.boundingClientRect.top = node_top;
    visibilityInfo.rect.boundingClientRect.bottom = node_top + node_height;
    visibilityInfo.rect.boundingClientRect.width = node_width;
    visibilityInfo.rect.boundingClientRect.height = node_height;

    // 设置relativeRect
    visibilityInfo.rect.relativeRect.left = viewport_x;
    visibilityInfo.rect.relativeRect.right = viewport_x + viewport_width;
    visibilityInfo.rect.relativeRect.top = viewport_y;
    visibilityInfo.rect.relativeRect.bottom = viewport_y + viewport_height;
}

// TODO: 另外，目前这种计算只适用于视窗，在适配 relativeTo 方法的时候需要调整
VisibilityInfo VisibleEventListener::CalculateNodeVisibility(
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> element,
    const CallbackInfo& callback_info) const {
    VisibilityInfo visibilityInfo;

    if (!element)
        return visibilityInfo;
    auto render_node = element->GetHeadRenderNode();
    if (!render_node)
        return visibilityInfo;

    float node_width = render_node->layoutDiffer_.computed_style_.width;
    float node_height = render_node->layoutDiffer_.computed_style_.height;
    auto position = render_node->calcGlobalPostion();

    if (position.is_hidden || position.is_lazy_index_outside) {
        return visibilityInfo;
    }

    // TARO_LOG_DEBUG("CalculateNodeVisibility", "%{public}d %{public}f %{public}f %{public}f %{public}f", element->nid_, position.global_x, position.global_y, position.offset_x, position.offset_y);

    updateVisibilityInfo(
        callback_info,
        position.global_x + position.offset_x,
        position.global_y + position.offset_y,
        node_width,
        node_height,
        visibilityInfo);

    return visibilityInfo; // 返回包含 intersectionRatio 和 rect 的结构体
}

VisibilityInfo VisibleEventListener::CalculateNodeVisibility(const ArkUI_NodeComponentEvent* compEvent,
                                                             const CallbackInfo& callback_info) const {
    VisibilityInfo visibilityInfo;

    float node_width = compEvent->data[6].f32;
    float node_height = compEvent->data[7].f32;

    float node_left = compEvent->data[10].f32;
    float node_top = compEvent->data[11].f32;

    updateVisibilityInfo(callback_info, node_left, node_top,
                         node_width, node_height, visibilityInfo);

    return visibilityInfo;
}
