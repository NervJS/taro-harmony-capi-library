//
// Created on 2024/5/15.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "TaroYogaApi.h"

#include <stack>

#include "helper/StringUtils.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "yoga/YGNodeLayout.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
using namespace TaroCSSOM;

TaroYogaApi* TaroYogaApi::instance = nullptr;
TaroYogaApi* TaroYogaApi::getInstance() {
    if (instance == nullptr) {
        instance = new TaroYogaApi();
    }
    return instance;
}

TaroYogaApi::TaroYogaApi() {
    ygConfig = YGConfigNew();
    YGConfigSetUseWebDefaults(ygConfig, true);
    // 缩放因子，我们入参到YG的是VP，实际VP与真实物理像素PX存在一个比值，为了消除浮点数运算对测量结果的精度问题，需要设置vp -> px的缩放因子
    YGConfigSetPointScaleFactor(ygConfig, DimensionContext::GetInstance()->density_pixels_ * 1000);
}

TaroYogaApi::~TaroYogaApi() {
    if (ygConfig != nullptr) {
        YGConfigFree(ygConfig);
        ygConfig = nullptr;
    }
}

YGNodeRef TaroYogaApi::createYogaNodeRef() {
    return YGNodeNewWithConfig(ygConfig);
}

void TaroYogaApi::calcYGLayout(YGNodeRef node) {
    if (node) {
        YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);
    }
}

void TaroYogaApi::calcYGLayout(YGNodeRef node, float ownerWidth,
                               float ownerHeight) {
    if (node) {
        YGNodeCalculateLayout(node, ownerWidth, ownerHeight, YGDirectionLTR);
    }
}

void TaroYogaApi::insertChildToParent(YGNodeRef parent, YGNodeRef child,
                                      size_t index) {
    if (child) {
        auto oddParent = YGNodeGetParent(child);
        if (oddParent) {
            removeChild(oddParent, child);
        }
        if (parent && !YGNodeHasMeasureFunc(parent)) {
            int i = YGNodeGetChildCount(parent);
            YGNodeInsertChild(parent, child, i >= index ? index : i);
        }
    }
}

int YGNodeGetChildIndex(YGNodeRef parent, YGNodeRef child) {
    if (!parent) return -1;
    int childCount = YGNodeGetChildCount(parent);
    for (int i = 0; i < childCount; i++) {
        if (YGNodeGetChild(parent, i) == child) {
            return i;
        }
    }
    return -1;
}

void TaroYogaApi::removeChild(YGNodeRef parent, YGNodeRef child) {
    if (parent && child) {
        YGNodeRemoveChild(parent, child);
    }
}

void TaroYogaApi::replaceChild(YGNodeRef parent, YGNodeRef newNodeRef,
                               YGNodeRef oldNodeRef) {
    auto childIndex = YGNodeGetChildIndex(parent, oldNodeRef);
    if (childIndex != -1) {
        if (parent && newNodeRef) {
            YGNodeInsertChild(parent, newNodeRef, childIndex);
        }
    }
}

void TaroYogaApi::setMeasureFunc(YGNodeRef node,
                                 const YGMeasureFunc yGMeasureFunc) {
    YGNodeSetMeasureFunc(node, yGMeasureFunc);
}

void TaroYogaApi::setContext(YGNodeRef node, void* context) {
    YGNodeSetContext(node, context);
}

void* TaroYogaApi::getContext(YGNodeConstRef node) {
    return YGNodeGetContext(node);
}

bool TaroYogaApi::hasMeasureFunc(YGNodeRef node) {
    return YGNodeHasMeasureFunc(node);
}

float TaroYogaApi::getCalcLayoutWidth(YGNodeRef node) {
    return YGNodeLayoutGetWidth(node);
}

void TaroYogaApi::setCalcLayoutWidth(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode) {
    float width = YGNodeLayoutGetWidth(ygNode);
    TaroStylesheet::HarmonyStyleSetter::setWidth(arkNode, static_cast<double>(width));
}

float TaroYogaApi::getCalcLayoutHeight(YGNodeRef node) {
    return YGNodeLayoutGetHeight(node);
}

void TaroYogaApi::setCalcLayoutHeight(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode) {
    float height = YGNodeLayoutGetHeight(ygNode);
    TaroStylesheet::HarmonyStyleSetter::setHeight(arkNode, static_cast<double>(height));
}

void TaroYogaApi::setCalcLayoutPosition(YGNodeRef ygNode,
                                        ArkUI_NodeHandle& arkNode) {
    float top = YGNodeLayoutGetTop(ygNode);
    float left = YGNodeLayoutGetLeft(ygNode);
    // TODO：设置位置
    ArkUI_NumberValue arkUI_NumberValue[] = {0.0, 0.0};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
    arkUI_NumberValue[0].f32 = left;
    arkUI_NumberValue[1].f32 = top;
    NativeNodeApi::getInstance()->setAttribute(arkNode, NODE_POSITION, &item);
}

void TaroYogaApi::setCalcLayoutMargin(YGNodeRef ygNode,
                                      ArkUI_NodeHandle& arkNode) {
    float marginTop = YGNodeLayoutGetMargin(ygNode, YGEdgeTop);
    float marginBottom = YGNodeLayoutGetMargin(ygNode, YGEdgeBottom);
    float marginLeft = YGNodeLayoutGetMargin(ygNode, YGEdgeLeft);
    float marginRight = YGNodeLayoutGetMargin(ygNode, YGEdgeRight);

    TaroStylesheet::HarmonyStyleSetter::setMargin(
        arkNode, marginTop, marginRight, marginBottom, marginLeft);
}

void TaroYogaApi::setCalcLayoutPadding(YGNodeRef ygNode,
                                       ArkUI_NodeHandle& arkNode) {
    float paddingTop = YGNodeLayoutGetPadding(ygNode, YGEdgeTop);
    float paddingBottom = YGNodeLayoutGetPadding(ygNode, YGEdgeBottom);
    float paddingLeft = YGNodeLayoutGetPadding(ygNode, YGEdgeLeft);
    float paddingRight = YGNodeLayoutGetPadding(ygNode, YGEdgeRight);

    TaroStylesheet::HarmonyStyleSetter::setPadding(
        arkNode, paddingTop, paddingRight, paddingBottom, paddingLeft);
}

void TaroYogaApi::setCalcLayoutBorder(YGNodeRef ygNode,
                                      ArkUI_NodeHandle& arkNode) {
    float borderTop = YGNodeLayoutGetBorder(ygNode, YGEdgeTop);
    float borderBottom = YGNodeLayoutGetBorder(ygNode, YGEdgeBottom);
    float borderLeft = YGNodeLayoutGetBorder(ygNode, YGEdgeLeft);
    float borderRight = YGNodeLayoutGetBorder(ygNode, YGEdgeRight);

    TaroStylesheet::HarmonyStyleSetter::setBorderWidth(
        arkNode, borderTop, borderRight, borderBottom, borderLeft);
}

void TaroYogaApi::setCalcLayoutDirection(YGNodeRef ygNode,
                                         ArkUI_NodeHandle& arkNode) {
    auto direction = YGNodeLayoutGetDirection(ygNode);
    ArkUI_NumberValue arkUI_NumberValue[] = {0.0};
    ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
    switch (direction) {
        case YGDirectionLTR:
            arkUI_NumberValue[0].i32 = ARKUI_DIRECTION_LTR;
            break;
        case YGDirectionRTL:
            arkUI_NumberValue[0].i32 = ARKUI_DIRECTION_RTL;
            break;
        default:
            arkUI_NumberValue[0].i32 = ARKUI_DIRECTION_AUTO;
            break;
    }
    NativeNodeApi::getInstance()->setAttribute(arkNode, NODE_DIRECTION, &item);
}

float TaroYogaApi::getCalcLayoutPaddingLeft(YGNodeRef ygNode) {
    return YGNodeLayoutGetPadding(ygNode, YGEdge::YGEdgeLeft);
}

float TaroYogaApi::getCalcLayoutPaddingTop(YGNodeRef ygNode) {
    return YGNodeLayoutGetPadding(ygNode, YGEdge::YGEdgeTop);
}

float TaroYogaApi::getCalcLayoutPaddingRight(YGNodeRef ygNode) {
    return YGNodeLayoutGetPadding(ygNode, YGEdge::YGEdgeRight);
}

float TaroYogaApi::getCalcLayoutPaddingBottom(YGNodeRef ygNode) {
    return YGNodeLayoutGetPadding(ygNode, YGEdge::YGEdgeBottom);
}

void TaroYogaApi::markDirty(YGNodeRef ygNode) {
    try {
        YGNodeMarkDirty(ygNode);
    } catch (const std::exception& e) {
        TARO_LOG_ERROR("TaroYoga", "markDirty error: %{public}s", e.what());
    }
}

void TaroYogaApi::markDirtyNodesWithCustomMeasureFunction(const YGNodeRef root) {
    if (root == nullptr) return;

    std::stack<YGNodeRef> stack;
    stack.push(root);

    while (!stack.empty()) {
        YGNodeRef node = stack.top();
        stack.pop();

        if (hasMeasureFunc(node)) {
            markDirty(node);
        }

        for (size_t i = 0; i < YGNodeGetChildCount(node); ++i) {
            stack.push(YGNodeGetChild(node, i));
        }
    }
}

float TaroYogaApi::getParentNodeWidthNotZero(const YGNodeRef ygNode) {
    if (ygNode == nullptr) return 0;
    YGNodeRef currentNode = ygNode;
    while (currentNode != nullptr) {
        float width = YGNodeLayoutGetWidth(currentNode);
        if (width != 0) {
            return width;
        }
        currentNode = YGNodeGetOwner(currentNode);
    }
    return 0;
}

bool TaroYogaApi::hasAncestorWithDisplayNone(const YGNodeRef ygNode) {
    if (ygNode == nullptr) {
        return false;
    }
    YGNodeRef currentNode = YGNodeGetOwner(ygNode);
    while (currentNode != nullptr) {
        YGDisplay display = YGNodeStyleGetDisplay(currentNode);
        if (display == YGDisplayNone) {
            return true;
        }
        currentNode = YGNodeGetOwner(currentNode);
    }
    return false;
}

} // namespace TaroRuntime