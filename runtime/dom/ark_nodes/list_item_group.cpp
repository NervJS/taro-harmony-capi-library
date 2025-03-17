//
// Created on 2024/8/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "list_item_group.h"

#include "list.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/dom/element/list.h"
#include "yoga/YGNodeStyle.h"
#include "yoga/YGValue.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroListItemGroupNode::TaroListItemGroupNode(const TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroListItemGroupNode::~TaroListItemGroupNode() {}

    void TaroListItemGroupNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_LIST_ITEM_GROUP));
    }

    void TaroListItemGroupNode::LayoutSelf() {
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

    void TaroListItemGroupNode::setHeader(ArkUI_NodeHandle handle) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.object = handle};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_LIST_ITEM_GROUP_SET_HEADER, &item);
    }

    void TaroListItemGroupNode::setFooter(ArkUI_NodeHandle handle) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.object = handle};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_LIST_ITEM_GROUP_SET_FOOTER, &item);
    }

    void TaroListItemGroupNode::setSpace(float space) {
        ArkUI_NumberValue value[1] = {};
        ArkUI_AttributeItem item = {value, 1};
        value[0].f32 = space;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_LIST_SPACE, &item);
    }

    void TaroListItemGroupNode::Layout() {
        if (!is_first_layout_finish_) {
            TaroRenderNode::Layout();
            return;
        }
        auto width_old = layoutDiffer_.computed_style_.width;
        auto height_old = layoutDiffer_.computed_style_.height;
        TaroRenderNode::Layout();
        auto width_new = layoutDiffer_.computed_style_.width;
        auto height_new = layoutDiffer_.computed_style_.height;
        if (!NearEqual(width_old, width_new, 1.0f) || !NearEqual(height_old, height_new, 1.0f)) {
            auto parent = std::dynamic_pointer_cast<TaroListNode>(parent_ref_.lock());
            if (parent) {
                parent->adapter_->reloadItem(element_ref_.lock());
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime