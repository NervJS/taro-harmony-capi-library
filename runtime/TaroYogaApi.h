/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_TAROYOGAAPI_H
#define TARO_CAPI_HARMONY_DEMO_TAROYOGAAPI_H

#include <memory>

#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/dom/element/element.h"
#include "yoga/YGConfig.h"
#include "yoga/YGNodeStyle.h"
#include "yoga/node/Node.h"

namespace TaroRuntime {
class TaroYogaApi {
    private:
    static TaroYogaApi* instance;
    YGConfigRef ygConfig = nullptr;
    TaroYogaApi();
    ~TaroYogaApi();

    public:
    TaroYogaApi(const TaroYogaApi&) = delete;
    TaroYogaApi& operator=(const TaroYogaApi&) = delete;

    static TaroYogaApi* getInstance();

    YGNodeRef createYogaNodeRef();
    void calcYGLayout(YGNodeRef node);
    void calcYGLayout(YGNodeRef node, float ownerWidth, float ownerHeight);
    void insertChildToParent(YGNodeRef parent, YGNodeRef child, size_t index);
    void removeChild(YGNodeRef parent, YGNodeRef child);
    void replaceChild(YGNodeRef parent, YGNodeRef newNodeRef,
                      YGNodeRef oldNodeRef);

    void setMeasureFunc(YGNodeRef node, const YGMeasureFunc);
    void setContext(YGNodeRef node, void* context);
    void* getContext(YGNodeConstRef node);
    bool hasMeasureFunc(YGNodeRef node);

    float getCalcLayoutWidth(YGNodeRef ygNode);
    void setCalcLayoutWidth(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    float getCalcLayoutHeight(YGNodeRef ygNode);
    void setCalcLayoutHeight(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    void setCalcLayoutPosition(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    void setCalcLayoutMargin(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    void setCalcLayoutPadding(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    void setCalcLayoutBorder(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    void setCalcLayoutDirection(YGNodeRef ygNode, ArkUI_NodeHandle& arkNode);
    float getCalcLayoutPaddingLeft(YGNodeRef ygNode);
    float getCalcLayoutPaddingTop(YGNodeRef ygNode);
    float getCalcLayoutPaddingRight(YGNodeRef ygNode);
    float getCalcLayoutPaddingBottom(YGNodeRef ygNode);

    void markDirty(YGNodeRef ygNode);
    void markDirtyNodesWithCustomMeasureFunction(const YGNodeRef ygNode);
    float getParentNodeWidthNotZero(const YGNodeRef ygNode);
    bool hasAncestorWithDisplayNone(const YGNodeRef ygNode);
};
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_TAROYOGAAPI_H
