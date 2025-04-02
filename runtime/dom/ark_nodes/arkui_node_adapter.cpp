/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "arkui_node_adapter.h"

#include <cstdint>
#include <functional>
#include <string>

#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/ark_nodes/flow_item.h"
#include "runtime/dom/ark_nodes/list_item.h"
#include "runtime/dom/ark_nodes/list_item_group.h"
#include "runtime/dom/element/flow_item.h"
#include "runtime/dom/element/list_item.h"
#include "runtime/dom/element/list_item_group.h"
#include "runtime/render.h"
#include "yoga/YGNode.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroNodeAdapter::TaroNodeAdapter()
        : adapter_(OH_ArkUI_NodeAdapter_Create()) {
        // 设置懒加载回调事件。
        OH_ArkUI_NodeAdapter_RegisterEventReceiver(adapter_, this, onStaticAdapterEvent);
    }

    TaroNodeAdapter::~TaroNodeAdapter() {
        for (auto it = cachedItems_.begin(); it != cachedItems_.end(); ++it) {
            it->second.clear();
        }
        cachedItems_.clear();
        items_.clear();
        dataSource_.clear();
        OH_ArkUI_NodeAdapter_UnregisterEventReceiver(adapter_);
        OH_ArkUI_NodeAdapter_Dispose(adapter_);
    }

    ArkUI_NodeAdapterHandle TaroNodeAdapter::GetHandle() const {
        return adapter_;
    }

    int TaroNodeAdapter::removeItem(std::shared_ptr<TaroNode> taroNode) {
        auto it = find(dataSource_.begin(), dataSource_.end(), taroNode);
        if (it != dataSource_.end()) {
            taroNode->is_attached = false;
            dataSource_.erase(it);
            // 如果index会导致可视区域元素发生可见性变化，则会回调NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER事件删除元素，
            // 根据是否有新增元素回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID和NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件。
            auto index = std::distance(dataSource_.begin(), it);
            OH_ArkUI_NodeAdapter_RemoveItem(adapter_, index, 1);
            auto element = std::dynamic_pointer_cast<TaroElement>(taroNode);
            auto renderNode = element->GetHeadRenderNode();
            if (renderNode && !element->checkIsInScrollContainerIndex()) {
                clearRemovedData(renderNode);
            }
            // 更新数量。
            OH_ArkUI_NodeAdapter_SetTotalNodeCount(adapter_, dataSource_.size());
            return 0;
        }
        return -1;
    }

    void TaroNodeAdapter::insertItem(std::shared_ptr<TaroNode> taroNode) {
        insertItemAt(taroNode, dataSource_.size());
    }

    int TaroNodeAdapter::insertItemAt(std::shared_ptr<TaroNode> taroNode, int32_t index) {
        if (index > dataSource_.size()) {
            TARO_LOG_ERROR("TaroNodeAdapter", "invalid index:%{public}d, count:%{public}d", index, dataSource_.size());
            return -1;
        }
        taroNode->is_attached = false;
        dataSource_.insert(dataSource_.begin() + index, taroNode);
        // 如果index会导致可视区域元素发生可见性变化，则会回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID和NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件，
        // 根据是否有删除元素回调NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER事件。
        OH_ArkUI_NodeAdapter_InsertItem(adapter_, index, 1);
        // 更新新的数量。
        OH_ArkUI_NodeAdapter_SetTotalNodeCount(adapter_, dataSource_.size());
        return 0;
    }

    void TaroNodeAdapter::insertItemBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        auto it = find(dataSource_.begin(), dataSource_.end(), refChild);
        auto index = std::distance(dataSource_.begin(), it);

        child->is_attached = false;
        auto old = find(dataSource_.begin(), dataSource_.end(), child);
        if (old != dataSource_.end()) {
            int oldIndex = std::distance(dataSource_.begin(), old);
            moveItem(oldIndex, index);
            return;
        }

        dataSource_.insert(it, child);
        // 如果index会导致可视区域元素发生可见性变化，则会回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID和NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件，
        // 根据是否有删除元素回调NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER事件。
        OH_ArkUI_NodeAdapter_InsertItem(adapter_, index, 1);
        // 更新数量。
        OH_ArkUI_NodeAdapter_SetTotalNodeCount(adapter_, dataSource_.size());
    }

    void TaroNodeAdapter::moveItem(int32_t oldIndex, int32_t newIndex) {
        // 移到位置如果未发生可视区域内元素的可见性变化，则不回调事件，反之根据新增和删除场景回调对应的事件。
        auto tmp = dataSource_[oldIndex];
        if (oldIndex < newIndex) {
            // 插入后删除
            dataSource_.insert(dataSource_.begin() + newIndex, tmp);
            dataSource_.erase(dataSource_.begin() + oldIndex);
        } else {
            // 删除后插入
            dataSource_.erase(dataSource_.begin() + oldIndex);
            dataSource_.insert(dataSource_.begin() + newIndex, tmp);
        }
        OH_ArkUI_NodeAdapter_MoveItem(adapter_, oldIndex, newIndex);
    }

    void TaroNodeAdapter::reloadItem(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {
        auto it = find(dataSource_.begin(), dataSource_.end(), oldChild);
        if (it == dataSource_.end()) {
            TARO_LOG_DEBUG("TaroNodeAdapter", "We cannot find the element to be replaced");
            return;
        }
        auto index = std::distance(dataSource_.begin(), it);
        dataSource_[index].reset();
        dataSource_[index] = newChild;
        // 如果index位于可视区域内，先回调NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER删除老元素，
        // 再回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID和NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件。
        OH_ArkUI_NodeAdapter_ReloadItem(adapter_, index, 1);
    }

    void TaroNodeAdapter::reloadItem(std::shared_ptr<TaroNode> item) {
        auto it = find(dataSource_.begin(), dataSource_.end(), item);
        if (it == dataSource_.end()) {
            TARO_LOG_DEBUG("TaroNodeAdapter", "We cannot find the element to reload");
            return;
        }
        auto index = std::distance(dataSource_.begin(), it);
        // 如果index位于可视区域内，先回调NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER删除老元素，
        // 再回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID和NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件。
        TARO_LOG_DEBUG("TaroNodeAdapter", "item:%{public}d reloaded", index);
        OH_ArkUI_NodeAdapter_ReloadItem(adapter_, index, 1);
    }

    void TaroNodeAdapter::reloadAllItem() {
        // 全部重新加载场景下，会回调NODE_ADAPTER_EVENT_ON_GET_NODE_ID接口获取新的组件ID，
        // 根据新的组件ID进行对比，ID不发生变化的进行复用，
        // 针对新增ID的元素，调用NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER事件创建新的组件，
        // 然后判断老数据中遗留的未使用ID，调用NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER删除老元素。
        OH_ArkUI_NodeAdapter_ReloadAllItems(adapter_);
    }

    // 分配ID给需要显示的Item，用于ReloadAllItems场景的元素diff，注意:要保证id唯一性!!
    void TaroNodeAdapter::onNewItemIdCreated(ArkUI_NodeAdapterEvent *event) {
        auto index = OH_ArkUI_NodeAdapterEvent_GetItemIndex(event);
        if (index >= dataSource_.size()) {
            TARO_LOG_DEBUG("TaroNodeAdapter", "There is no more data to display");
            return;
        }
        auto taroNode = dataSource_[index];
        auto taroElement = std::dynamic_pointer_cast<TaroDOM::TaroElement>(taroNode);

        auto item_id = std::to_string(index) + "#";
        if (taroElement != nullptr) {
            item_id += std::to_string(taroElement->nid_);
        }
        static std::hash<std::string> hashId = std::hash<std::string>();
        auto id = hashId(item_id);
        OH_ArkUI_NodeAdapterEvent_SetNodeId(event, id);
    }

    void TaroNodeAdapter::setRenderNode(std::shared_ptr<TaroRenderNode> &renderNode) {
        renderNode_ = renderNode;
    }

    int32_t TaroNodeAdapter::getDataSize() {
        return dataSource_.size();
    }

    void TaroNodeAdapter::initArkUINode(std::shared_ptr<TaroElement> &newElement,
                                        std::shared_ptr<TaroElement> &reuseElement) {
        newElement->is_reused_ = false;
        newElement->PreBuild();
        newElement->Build(reuseElement);
        newElement->PostBuild();
        reuseElement->is_reused_ = true;
    }

    void TaroNodeAdapter::buildNewNode(std::shared_ptr<TaroElement> &newElement) {
        std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
        stack.push(newElement);

        while (!stack.empty()) {
            auto parent = stack.top();
            stack.pop();
            parent->SetStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
            if (parent->is_reused_ && parent->GetNodeHandle() == nullptr) {
                parent->PreBuild();
                parent->Build(parent);
                parent->PostBuild();
            } else if (parent->GetNodeHandle()) {
                if (auto newParent = parent->GetParentNode()->GetHeadRenderNode()) {
                    newParent->UpdateChild(parent->GetHeadRenderNode());
                }
            } else {
                parent->BuildProcess();
            }
            parent->ClearStateFlag(TaroRuntime::STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
            auto childNodes = parent->child_nodes_;
            for (int i = childNodes.size() - 1; i >= 0; --i) {
                auto child = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
                if (child) {
                    stack.push(child);
                }
            }
        }
    }

    void TaroNodeAdapter::destroyUnusedNode(std::shared_ptr<TaroElement> &newParent,
                                            std::shared_ptr<TaroElement> &reuseElement) {
        auto reuseParent = reuseElement->GetParentNode();
        if (reuseParent == nullptr) {
            return;
        }
        bool isReused = false;
        if (reuseParent->GetFooterRenderNode()->GetArkUINodeHandle() == nullptr) {
            isReused = true;
            reuseParent->GetFooterRenderNode()->SetArkUINodeHandle(newParent->GetFooterRenderNode()->GetArkUINodeHandle());
        }
        reuseElement->SetStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
        reuseParent->onDeleteRenderNode(reuseElement);
        if (isReused) {
            reuseParent->GetFooterRenderNode()->SetArkUINodeHandle(nullptr);
        }
        reuseElement->ClearStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
    }

    void TaroNodeAdapter::initChildNode(std::vector<std::shared_ptr<TaroNode>> &newNode,
                                        std::shared_ptr<TaroElement> &newParent,
                                        std::vector<std::shared_ptr<TaroNode>> &reuseNode) {
        int newIndex = newNode.size();
        int oldIndex = reuseNode.size();
        int index = 0;
        for (index; index < newIndex && index < oldIndex; index++) {
            auto newChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(newNode[index]);
            auto reuseChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(reuseNode[index]);
            initRenderNode(newChild, reuseChild);
        }
        while (index < newIndex) {
            auto newChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(newNode[index]);
            buildNewNode(newChild);
            index++;
        }
        while (index < oldIndex) {
            auto reuseChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(reuseNode[index]);
            destroyUnusedNode(newParent, reuseChild);
            index++;
        }
    }

    // DFS初始化子组件
    void TaroNodeAdapter::initRenderNode(std::shared_ptr<TaroElement> &newElement,
                                         std::shared_ptr<TaroElement> &reuseElement) {
        if (reuseElement == newElement) {
            return;
        }
        TARO_LOG_DEBUG("TaroNodeAdapter", "new tag name:%{public}s, old tag name:%{public}s",
            newElement->GetNodeName().c_str(), reuseElement->GetNodeName().c_str());
        if (newElement->Reusable(reuseElement)) {
            initArkUINode(newElement, reuseElement);
            initChildNode(newElement->child_nodes_, newElement, reuseElement->child_nodes_);
        } else {
            buildNewNode(newElement);
            auto newParent = std::dynamic_pointer_cast<TaroDOM::TaroElement>(newElement->GetParentNode());
            destroyUnusedNode(newParent, reuseElement);
        }
    }

    bool TaroNodeAdapter::isInitializedElement(std::shared_ptr<TaroElement> &newElement) {
        std::string reuseId = newElement->reuse_id_;
        auto node = newElement->GetHeadRenderNode();
        if (reuseId.empty() || node == nullptr) {
            return false;
        }
        auto handle = node->GetArkUINodeHandle();
        if (handle != nullptr) {
            auto mapIt = cachedItems_.find(reuseId);
            if (mapIt != cachedItems_.end()) {
                auto setIt = mapIt->second.find(node);
                if (setIt != mapIt->second.end()) {
                    mapIt->second.erase(setIt);
                }
            }
            return true;
        }
        return false;
    }

    void TaroNodeAdapter::appendToNewParent(std::shared_ptr<TaroElement> &newElement) {
        auto newParent = newElement->GetParentNode();
        if (newParent != nullptr) {
            if (!newParent->lazy_node) {
                NativeNodeApi::getInstance()->addChild(newParent->GetHeadRenderNode()->GetArkUINodeHandle(),
                    newElement->GetHeadRenderNode()->GetArkUINodeHandle());
            }
        }
    }

    void TaroNodeAdapter::removeFromReuseParent(std::shared_ptr<TaroElement> &newElement,
                                                std::shared_ptr<TaroElement> &reuseElement) {
        auto oldParent = reuseElement->GetParentNode();
        if (oldParent != nullptr) {
            if (!oldParent->lazy_node) {
                bool isReused = false;
                if (reuseElement->GetHeadRenderNode()->GetArkUINodeHandle() == nullptr) {
                    isReused = true;
                    reuseElement->GetHeadRenderNode()->SetArkUINodeHandle(
                        newElement->GetHeadRenderNode()->GetArkUINodeHandle());
                }
                NativeNodeApi::getInstance()->removeChild(oldParent->GetHeadRenderNode()->GetArkUINodeHandle(),
                    reuseElement->GetHeadRenderNode()->GetArkUINodeHandle());
                if (isReused) {
                    reuseElement->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
                }
            }
        }
    }

    void TaroNodeAdapter::recycleManager(std::shared_ptr<TaroElement> &taroElement, bool need_match_style) {
        std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
        stack.push(taroElement);
        while (!stack.empty()) {
            auto newElement = stack.top();
            stack.pop();
            // 给 item 标记是否包含复用单元
            if (!taroElement->reusable_ && !newElement->reuse_id_.empty()) {
                taroElement->reusable_ = true;
            }
            if (isInitializedElement(newElement)) {
                updateStyle(need_match_style, newElement);
                continue;
            }
            if (need_match_style) {
                newElement->MatchStylesheet();
            }
            auto it = cachedItems_.find(newElement->reuse_id_);
            if (it != cachedItems_.end() && !(it->second.empty())) {
                auto reuseIt = it->second.begin();
                auto reuseElement = (*reuseIt)->element_ref_.lock();

                if (reuseElement == nullptr) {
                    it->second.erase(reuseIt);
                    stack.push(newElement);
                    continue;
                }
                initRenderNode(newElement, reuseElement);
                it->second.erase(reuseIt);
                removeFromReuseParent(newElement, reuseElement);
                appendToNewParent(newElement);
            } else {
                newElement->SetStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
                if (newElement->is_reused_ && newElement->GetNodeHandle() == nullptr) {
                    newElement->PreBuild();
                    newElement->Build(newElement);
                    newElement->PostBuild();
                } else if (newElement->GetNodeHandle()) {
                    if (auto newParent = newElement->GetParentNode()->GetHeadRenderNode()) {
                        newParent->UpdateChild(newElement->GetHeadRenderNode());
                    }
                } else {
                    newElement->BuildProcess();
                }
                newElement->ClearStateFlag(STATE_FLAG::IMMEDIATE_ATTACH_TO_TREE);
                if (newElement->lazy_node) {
                    continue;
                }
                auto childNodes = newElement->child_nodes_;
                for (int i = childNodes.size() - 1; i >= 0; --i) {
                    auto child = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
                    if (child) {
                        stack.push(child);
                    }
                }
            }
        }
    }

    // 需要新的Item显示在可见区域。创建/复用节点
    void TaroNodeAdapter::onNewItemAttached(ArkUI_NodeAdapterEvent *event) {
        auto index = OH_ArkUI_NodeAdapterEvent_GetItemIndex(event);
        if (index >= dataSource_.size()) {
            TARO_LOG_ERROR("TaroNodeAdapter", "There is no more data to display");
            return;
        }
        // 待展示数据源
        auto taroNode = dataSource_[index];
        auto taroElement = std::dynamic_pointer_cast<TaroDOM::TaroElement>(taroNode);
        if (taroElement == nullptr) {
            TARO_LOG_ERROR("TaroNodeAdapter", "taro element is nullptr");
            return;
        }
        bool need_match_style = taroElement->is_need_match_style;
        bool need_rebuild = !taroNode->is_attached || need_match_style || taroElement->reusable_;
        // taro element可能是 listItem,listItemGroup,flowItem
        if (need_rebuild) {
            dirtyFlagNode_ = nullptr;
            checkParentDirty();
            recycleManager(taroElement, need_match_style);
            clearParentDirty();
        }
        taroElement->is_need_match_style = false;
        auto renderNode = taroElement->GetHeadRenderNode();
        if (need_rebuild && renderNode) {
            if (auto flowItem = std::dynamic_pointer_cast<TaroFlowItemNode>(renderNode)) {
                flowItem->LayoutSelf();
            } else if (auto listGroup = std::dynamic_pointer_cast<TaroListItemGroupNode>(renderNode)) {
                listGroup->LayoutSelf();
            } else if (auto listItem = std::dynamic_pointer_cast<TaroListItemNode>(renderNode)) {
                listItem->LayoutSelf();
            }
            if (!taroElement->reusable_) {
                taroNode->is_attached = true;
            }
        }
        taroElement->is_need_match_style = false;
        DirtyTaskPipeline::GetInstance()->FlushPaintLocalSync(taroElement);

        ArkUI_NodeHandle handle = nullptr;
        if (renderNode != nullptr) {
            handle = renderNode->GetArkUINodeHandle();
        }
        if (handle == nullptr) {
            TARO_LOG_ERROR("TaroNodeAdapter", "create or reuse arkui node failed:%{public}d", index);
            return;
        }

        TARO_LOG_DEBUG("TaroNodeAdapter", "index:%{public}d,item width:%{public}f,height:%{public}f",
                       index, renderNode->layoutDiffer_.computed_style_.width, renderNode->layoutDiffer_.computed_style_.height);

        // 子组件会更新，我们只保存最新结构
        items_.insert_or_assign(renderNode->GetArkUINodeHandle(), renderNode);

        // 设置需要展示的元素。
        int res = OH_ArkUI_NodeAdapterEvent_SetItem(event, handle);
        if (res != 0) {
            TARO_LOG_ERROR("TaroNodeAdapter", "node adapter set item failed %{public}d", res);
        }
    }

    void TaroNodeAdapter::measureAndLayout() {
        auto render_shared = renderNode_.lock();
        if (render_shared == nullptr) {
            return;
        }
        measureAndLayout(render_shared);
    }

    void TaroNodeAdapter::measureAndLayout(std::shared_ptr<TaroRenderNode> renderNode) {
        if (renderNode) {
            renderNode->MeasureAndLayoutLocal();
        }
    }

    void TaroNodeAdapter::onItemDeleted(ArkUI_NodeAdapterEvent *event) {
        auto item = OH_ArkUI_NodeAdapterEvent_GetRemovedNode(event);
        auto renderNode = items_[item];
        if (renderNode == nullptr) {
            return;
        }
        auto element = renderNode->element_ref_.lock();
        // 处理由OH_ArkUI_NodeAdapter_RemoveItem触发的可视性变化，这部分节点已被上层回收
        // 需要从items中删除对应的render node
        if (element && element->GetParentNode() != nullptr) {
            if (!element->reusable_) {
                TARO_LOG_DEBUG("TaroNodeAdapter", "item cannot be reused: %{public}d", element->nid_);
                return;
            }
#if (ENABLE_REUSE_ARKNODE == 1)
            // TODO: 这里不应该写这个条件判断，但是目前会发现某些节点在触发 onNewItemAttach 后，又会马上触发 onItemDelete 事件，导致节点被卸载，出现空窗
            if (!element->checkIsInScrollContainerIndex()) {
                onItemRecycled(renderNode);
            }
#endif
        } else {
            // 处理由OH_ArkUI_NodeAdapter_RemoveItem触发的可视性变化，这部分节点已被上层回收
            // 需要从items中删除对应的render node
            renderNode.reset();
            items_.erase(item);
        }
    }

    void TaroNodeAdapter::onItemRecycled(std::shared_ptr<TaroRenderNode>& renderNode) {
        // 放置到缓存池中进行回收复用。
        std::stack<std::shared_ptr<TaroRenderNode>> stack;
        stack.push(renderNode);
        while (!stack.empty()) {
            auto node = stack.top();
            stack.pop();
            auto element = node->element_ref_.lock();
            if (!element->reuse_id_.empty()) {
                TARO_LOG_DEBUG("TaroNodeAdapter", "nid:%{public}d, reuse id:%{public}s", element->nid_, element->reuse_id_.c_str());
                // 暂存最大复用子组件
                if (node->GetArkUINodeHandle() != nullptr) {
                    cachedItems_[element->reuse_id_].emplace(node);
                }
            } else {
                auto childNodes = node->children_refs_;
                for (int i = childNodes.size() - 1; i >= 0; --i) {
                    stack.push(childNodes[i]);
                }
            }
        }
    }

    void TaroNodeAdapter::onStaticAdapterEvent(ArkUI_NodeAdapterEvent *event) {
        auto nodeAdapter = reinterpret_cast<TaroNodeAdapter *>(OH_ArkUI_NodeAdapterEvent_GetUserData(event));
        if (nodeAdapter != nullptr) {
            nodeAdapter->onAdapterEvent(event);
        }
    }

    void TaroNodeAdapter::onAdapterEvent(ArkUI_NodeAdapterEvent *event) {
        // 获取adapter事件类型
        auto type = OH_ArkUI_NodeAdapterEvent_GetType(event);
        switch (type) {
            case NODE_ADAPTER_EVENT_ON_GET_NODE_ID: {
                // 获取节点id
                onNewItemIdCreated(event);
                break;
            }
            case NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER: {
                // 添加节点到adapter
                onNewItemAttached(event);
                break;
            }
            case NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER: {
                onItemDeleted(event);
                break;
            }
            default:
                break;
        }
    }

    int32_t TaroNodeAdapter::getTaroNodeIndexInDataSource(std::shared_ptr<TaroNode> taroNode) {
        auto it = find(dataSource_.begin(), dataSource_.end(), taroNode);
        if (it != dataSource_.end()) {
            size_t index = std::distance(dataSource_.begin(), it);

            return index;
        }

        return -1;
    }

    void TaroNodeAdapter::checkParentDirty() {
        auto render_shared = renderNode_.lock();
        if (render_shared == nullptr) {
            return;
        }
        auto current = render_shared->parent_ref_.lock();
        while (current) {
            // 找到父级最近的脏节点
            if (YGNodeIsDirty(current->ygNodeRef)) {
                dirtyFlagNode_ = current;
                break;
            }
            current = current->parent_ref_.lock();
        }
    }

    void TaroNodeAdapter::clearParentDirty() {
        auto render_shared = renderNode_.lock();
        if (render_shared == nullptr) {
            return;
        }
        auto current = render_shared->parent_ref_.lock();
        while (current) {
            // 如果找到脏节点 dirtyFlagNode_，则退出
            if (current == dirtyFlagNode_) return;
            // 否则清理脏节点标记
            TaroYGNodeClearDirtyLocal(current->ygNodeRef);
            current = current->parent_ref_.lock();
        }
    }

    void TaroNodeAdapter::updateStyle(bool need_match_style, std::shared_ptr<TaroElement> &taroElement) {
        if (!need_match_style) {
            return;
        }
        std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
        stack.push(taroElement);
        while (!stack.empty()) {
            auto newElement = stack.top();
            stack.pop();

            newElement->MatchStylesheet();
            newElement->PostBuild();
            auto childNodes = newElement->child_nodes_;
            for (int i = childNodes.size() - 1; i >= 0; --i) {
                auto child = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
                if (child) {
                    stack.push(child);
                }
            }
        }
    }

    void TaroNodeAdapter::clearRemovedData(std::shared_ptr<TaroRenderNode> &renderNode) {
        auto handle = renderNode->GetArkUINodeHandle();
        auto it = items_.find(handle);
        if (it != items_.end()) {
            renderNode.reset();
            items_.erase(it);
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime
