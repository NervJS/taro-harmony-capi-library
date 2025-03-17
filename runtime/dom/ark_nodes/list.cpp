//
// Created on 2024/7/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "list.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/dom/ark_nodes/list_item.h"
#include "runtime/dom/ark_nodes/list_item_group.h"
#include "runtime/dom/element/list_item.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroListNode::TaroListNode(const TaroElementRef element)
        : TaroScrollContainerNode(element),
          adapter_(std::make_shared<TaroNodeAdapter>()),
          init_adapter_(false) {
        TaroSetCanMeasureChild(ygNodeRef, false);
    }

    TaroListNode::~TaroListNode() {
        if (init_adapter_) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            nativeNodeApi->resetAttribute(GetArkUINodeHandle(), NODE_LIST_NODE_ADAPTER);
        }
    }

    void TaroListNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_LIST));
        TaroScrollContainerNode::setEdgeEffect(ARKUI_EDGE_EFFECT_NONE);
    }

    void TaroListNode::setCacheCount(int32_t count) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[1] = {{.i32 = count}};
        ArkUI_AttributeItem item{value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_LIST_CACHED_COUNT, &item);
    }

    void TaroListNode::Layout() {
        TaroRenderNode::Layout();
        if (!init_adapter_) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            ArkUI_AttributeItem item{nullptr, 0, nullptr, adapter_->GetHandle()};
            nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_LIST_NODE_ADAPTER, &item);
            init_adapter_ = true;
            auto renderNode = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            adapter_->setRenderNode(renderNode);
        }
        LayoutSelf();
    }

    void TaroListNode::LayoutSelf() {
        for (auto child : children_refs_) {
            if (auto listItem = std::dynamic_pointer_cast<TaroListItemNode>(child)) {
                listItem->LayoutSelf();
            } else if (auto listItemGroup = std::dynamic_pointer_cast<TaroListItemGroupNode>(child)) {
                listItemGroup->LayoutSelf();
            }
        }
    }

    void TaroListNode::setStickyStyle(ArkUI_StickyStyle style) {
        ArkUI_NumberValue value[1] = {};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = style;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_LIST_STICKY, &item);
    }

    void TaroListNode::setSpace(float space) {
        ArkUI_NumberValue value[1] = {};
        ArkUI_AttributeItem item = {value, 1};
        value[0].f32 = space;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_LIST_SPACE, &item);
    }

} // namespace TaroDOM
} // namespace TaroRuntime