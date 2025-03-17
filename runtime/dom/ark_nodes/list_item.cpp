//
// Created on 2024/7/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "list_item.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/dom/element/list.h"
#include "yoga/YGNode.h"
#include "yoga/YGNodeStyle.h"
#include "yoga/YGValue.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroListItemNode::TaroListItemNode(const TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroListItemNode::~TaroListItemNode() {}

    void TaroListItemNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_LIST_ITEM));
    }

    void TaroListItemNode::LayoutSelf() {
        if (auto childElement = element_ref_.lock()) {
            if (auto listElement = std::dynamic_pointer_cast<TaroList>(childElement->GetParentNode())) {
                auto list = listElement->GetHeadRenderNode();
                float itemAvailableWidth = list->layoutDiffer_.computed_style_.width - list->layoutDiffer_.computed_style_.paddingLeft - list->layoutDiffer_.computed_style_.paddingRight;
                float itemAvailableHeight = YGUndefined;
                TaroSetCanMeasureChild(list->ygNodeRef, true);
                TaroYogaApi::getInstance()->calcYGLayout(ygNodeRef, itemAvailableWidth, itemAvailableHeight);
                YGNodeSetHasNewLayout(ygNodeRef, true);
                bool layoutWithoutDiff = false;
                if (childElement->reusable_) {
                    layoutWithoutDiff = true;
                }
                LayoutAll(layoutWithoutDiff);
                TaroSetCanMeasureChild(list->ygNodeRef, false);
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime