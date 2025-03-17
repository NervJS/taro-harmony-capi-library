//
// Created on 2024/7/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <unordered_set>
#include <arkui/native_node.h>

#include "arkui_node.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/node.h"

#ifndef ENABLE_REUSE_ARKNODE
#define ENABLE_REUSE_ARKNODE 1
#endif

namespace TaroRuntime {
namespace TaroDOM {
    class TaroNodeAdapter {
        public:
        TaroNodeAdapter();
        ~TaroNodeAdapter();

        ArkUI_NodeAdapterHandle GetHandle() const;
        int removeItem(std::shared_ptr<TaroNode> taroNode);
        // 在数据源尾部增加新数据
        void insertItem(std::shared_ptr<TaroNode> taroNode);
        // 在index位置插入新的数据源
        int insertItemAt(std::shared_ptr<TaroNode> taroNode, int32_t index);
        void insertItemBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild);
        void moveItem(int32_t oldIndex, int32_t newIndex);
        void reloadItem(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild);
        void reloadItem(std::shared_ptr<TaroNode> item);
        void reloadAllItem();
        void setRenderNode(std::shared_ptr<TaroRenderNode>& renderNode);
        int32_t getDataSize();
        int32_t getTaroNodeIndexInDataSource(std::shared_ptr<TaroNode> taroNode);

        private:
        static void onStaticAdapterEvent(ArkUI_NodeAdapterEvent* event);
        void onAdapterEvent(ArkUI_NodeAdapterEvent* event);
        void onNewItemIdCreated(ArkUI_NodeAdapterEvent* event);
        void onNewItemAttached(ArkUI_NodeAdapterEvent* event);
        void onItemRecycled(std::shared_ptr<TaroRenderNode>& renderNode);
        void onItemDeleted(ArkUI_NodeAdapterEvent* event);
        void measureAndLayout();
        void measureAndLayout(std::shared_ptr<TaroRenderNode> renderNode);
        void initRenderNode(std::shared_ptr<TaroElement>& newElement, std::shared_ptr<TaroElement>& reuseElement);
        void initArkUINode(std::shared_ptr<TaroElement>& newElement, std::shared_ptr<TaroElement>& reuseElement);
        void initChildNode(std::vector<std::shared_ptr<TaroNode>>& newNode, std::shared_ptr<TaroElement>& newParent,
                           std::vector<std::shared_ptr<TaroNode>>& reuseNode);

        void appendToNewParent(std::shared_ptr<TaroElement>& newElement);
        void removeFromReuseParent(std::shared_ptr<TaroElement>& newElement, std::shared_ptr<TaroElement>& reuseElement);

        void buildNewNode(std::shared_ptr<TaroElement>& newElement);
        void destroyUnusedNode(std::shared_ptr<TaroElement>& newElement, std::shared_ptr<TaroElement>& reuseElement);

        void recycleManager(std::shared_ptr<TaroElement>& taroElement, bool need_match_style);

        bool isInitializedElement(std::shared_ptr<TaroElement>& newElement);
        void updateStyle(bool need_match_style, std::shared_ptr<TaroElement> &taroElement);

        void checkParentDirty();
        void clearParentDirty();
        void clearRemovedData(std::shared_ptr<TaroRenderNode>& renderNode);

        ArkUI_NodeAdapterHandle adapter_;
        std::weak_ptr<TaroRenderNode> renderNode_;

        // 容器组件数据源
        std::vector<std::shared_ptr<TaroNode>> dataSource_;

        // 注意:要保持cachedItems_元素与items_中元素一致
        // 管理NodeAdapter生成的元素。
        std::unordered_map<ArkUI_NodeHandle, std::shared_ptr<TaroRenderNode>> items_;
        // key: reuseId
        std::unordered_map<std::string, std::unordered_set<std::shared_ptr<TaroRenderNode>>> cachedItems_;
        std::shared_ptr<TaroRenderNode> dirtyFlagNode_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime