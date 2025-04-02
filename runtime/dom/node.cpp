/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "node.h"

#include <memory>

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "helper/TaroLog.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/event/event_visible.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroNode::TaroNode(napi_value &node) {
        if (node == nullptr) return;

        ref_ = ArkJS(NativeNodeApi::env).createReference(node);
        nid_ = GetNodeId();
        node_type_ = GetNodeType();
        auto node_name = GetNodeName();
#if IS_DEBUG
        TARO_LOG_DEBUG("TaroNode", "构建 nid_: %{public}d; nodeType: %{public}d", nid_, node_type_);
#endif
    }

    TaroNode::~TaroNode() {
        // #if IS_DEBUG
        //         TARO_LOG_DEBUG("TaroNode", "析构 nid_: %{public}d; nodeType: %{public}d", nid_, node_type_);
        // #endif
        if (ref_) {
            ArkJS(NativeNodeApi::env).deleteReference(ref_);
        }

        for (auto &child : child_nodes_) {
            TaroDocument::GetInstance()->DetachTaroNode(child);
        }
        child_nodes_.clear();
        head_render_node_ = nullptr;
        footer_render_node_ = nullptr;
    }

    void TaroNode::SetParentNode(std::shared_ptr<TaroNode> parent_node) {
        parent_node_ = parent_node;
    }

    std::shared_ptr<TaroNode> TaroNode::GetParentNode() {
        return parent_node_.lock();
    }

    void TaroNode::SetPreviousSibling(
        std::shared_ptr<TaroNode> previous_sibling) {
        previous_sibling_ = previous_sibling;
    }

    std::shared_ptr<TaroNode> TaroNode::GetPreviousSibling() {
        return previous_sibling_.lock();
    }

    void TaroNode::SetNextSibling(std::shared_ptr<TaroNode> next_sibling) {
        next_sibling_ = next_sibling;
    }

    std::shared_ptr<TaroNode> TaroNode::GetNextSibling() {
        return next_sibling_.lock();
    }

    bool TaroNode::HasRenderNode() {
        return this->head_render_node_ != nullptr;
    }

    void TaroNode::SetRenderNode(std::shared_ptr<TaroRenderNode> render_node) {
        SetHeadRenderNode(render_node);
        SetFooterRenderNode(render_node);
    }

    void TaroNode::SetHeadRenderNode(
        std::shared_ptr<TaroRenderNode> head_render_node) {
        this->head_render_node_ = head_render_node;
    }

    void TaroNode::SetFooterRenderNode(
        std::shared_ptr<TaroRenderNode> footer_render_node) {
        this->footer_render_node_ = footer_render_node;
    }

    std::shared_ptr<TaroRenderNode> TaroNode::GetHeadRenderNode() {
        return this->head_render_node_;
    }

    std::shared_ptr<TaroRenderNode> TaroNode::GetFooterRenderNode() {
        return this->footer_render_node_;
    }

    ArkUI_NodeHandle TaroNode::GetNodeHandle() {
        auto renderNode = this->GetHeadRenderNode();
        if (renderNode != nullptr) {
            return renderNode->GetArkUINodeHandle();
        }
        return nullptr;
    }

    ArkUI_NodeHandle TaroNode::GetInnerNodeHandle() {
        auto renderNode = this->GetFooterRenderNode();
        if (renderNode != nullptr) {
            return renderNode->GetArkUINodeHandle();
        }
        return nullptr;
    }

    int32_t TaroNode::GetNodeId() {
        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        return GetNodeId(GetNodeValue());
    }

    int32_t TaroNode::GetNodeId(napi_value node) {
        return NapiGetter::GetProperty(node, "_nid").Int32Or(0);
    }

    NODE_TYPE TaroNode::GetNodeType() {
        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        return GetNodeType(GetNodeValue());
    }

    NODE_TYPE TaroNode::GetNodeType(napi_value node) {
        return static_cast<NODE_TYPE>(
            NapiGetter::GetProperty(node, "nodeType").Int32Or(0));
    }

    std::string TaroNode::GetNodeName() {
        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        return NapiGetter::GetProperty(GetNodeValue(), "nodeName").StringOr("");
    }

    napi_value TaroNode::GetNodeValue() {
        return ArkJS(NativeNodeApi::env).getReferenceValue(ref_);
    }

    void TaroNode::appendChild(std::shared_ptr<TaroNode> child) {
        if (!child) return;

        detachTaroNodeReference(child);

        if (!child_nodes_.empty()) {
            auto lastChild = child_nodes_.back();
            lastChild->SetNextSibling(child);
            child->SetPreviousSibling(lastChild);
        }
        child->SetParentNode(shared_from_this());
        child_nodes_.push_back(child);
        onAppendChild(child);
        TARO_LOG_DEBUG("TaroNode", "appendChild pid: %{public}d, cid: %{public}d", nid_, child->nid_);
    }

    void TaroNode::detachTaroNodeReference(std::shared_ptr<TaroNode> child) {
        auto parent = child->GetParentNode();

        if (parent == nullptr) return;

        /** begin: 将 child 的上下左右父子兄弟关系全部解绑 **/
        auto it = std::find(parent->child_nodes_.begin(), parent->child_nodes_.end(), child);
        if (it != parent->child_nodes_.end()) {
            if (it != parent->child_nodes_.begin()) {
                auto prev = std::prev(it);
                (*prev)->SetNextSibling(child->GetNextSibling());
            }
            if (std::next(it) != parent->child_nodes_.end()) {
                auto next = std::next(it);
                (*next)->SetPreviousSibling(child->GetPreviousSibling());
            }
            parent->child_nodes_.erase(it);
        }

        child->SetParentNode(nullptr);
        child->SetNextSibling(nullptr);
        child->SetPreviousSibling(nullptr);
        /** end: 将 child 的上下左右父子兄弟关系全部解绑 **/

        if (!child->is_init_) return;

        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto header = child->GetHeadRenderNode();
        // 已经构建过的 render node 节点不需要重新走 build
        if (header && TaroDocument::GetInstance()->findDOMPage(element)) {
            child->ignore_build = true;
            child->ignore_update_event = true;
        }

        // 将 render_node、yoga_node、ark_node 的上下左右父子兄弟关系解绑
        if (auto footer = parent->GetFooterRenderNode()) {
            footer->RemoveChild(child->GetHeadRenderNode());
        }

        // is_init_ 设置成 false，让其重新走后续的 prebuild 和 postbuild 流程
        child->is_init_ = false;
    }

    void TaroNode::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        // 递归卸载RenderNode
        for (auto item : child->child_nodes_) {
            if (item) {
                child->onRemoveChild(item, true);
            }
        }

        if (GetFooterRenderNode()) {
            ProcessRenderNodeFromTaroNode(child, [child, is_detach_render_node](std::weak_ptr<TaroRenderNode> childRef) {
                if (auto element = childRef.lock()) {
                    // 这种情况在 detachTaroNodeReference 中已经 remove 过了
                    if (!is_detach_render_node && element == child->GetHeadRenderNode()) {
                        return;
                    }
                    if (auto parent = element->parent_ref_.lock()) {
                        parent->RemoveChild(element);
                    }
                }
            });
        }

        if (child->nid_ != 0 && !is_detach_render_node) {
            TaroDocument::GetInstance()->DetachTaroNode(child);
        }
    }

    void TaroNode::removeChild(std::shared_ptr<TaroNode> child) {
        if (!child) return;

        detachTaroNodeReference(child);
        onRemoveChild(child, false);
        TARO_LOG_DEBUG("TaroNode", "removeChild pid: %{public}d, cid: %{public}d", nid_, child->nid_);
    }

    void TaroNode::replaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {
        if (!newChild) return;

        detachTaroNodeReference(newChild);
        TARO_LOG_DEBUG("TaroNode", "replaceChild pid: %{public}d, cid: %{public}d, odd-nid: %{public}d", nid_, newChild->nid_, oldChild->nid_);
        std::shared_ptr<TaroNode> oldChildParent = oldChild->GetParentNode();
        auto it = std::find(child_nodes_.begin(), child_nodes_.end(), oldChild);
        if (it != child_nodes_.end()) {
            newChild->SetPreviousSibling(oldChild->GetPreviousSibling());
            newChild->SetNextSibling(oldChild->GetNextSibling());
            if (newChild->GetPreviousSibling()) {
                newChild->GetPreviousSibling()->SetNextSibling(newChild);
            }
            if (newChild->GetNextSibling()) {
                newChild->GetNextSibling()->SetPreviousSibling(newChild);
            }

            child_nodes_.insert(it, newChild);
            newChild->SetParentNode(shared_from_this());
            oldChildParent->removeChild(oldChild);
        }

        onReplaceChild(newChild, oldChild);
        oldChild.reset();
        TARO_LOG_DEBUG("TaroNode", "replaceChild pid: %{public}d, cid: %{public}d", nid_, newChild->nid_);
    }

    void TaroNode::insertChildAt(std::shared_ptr<TaroNode> child, uint32_t index) {
        if (!child) return;

        detachTaroNodeReference(child);

        if (index < child_nodes_.size()) {
            auto it = child_nodes_.begin() + index;
            // Update the sibling pointers
            child->SetPreviousSibling((*it)->GetPreviousSibling());
            child->SetNextSibling(*it);
            if (child->GetPreviousSibling()) {
                child->GetPreviousSibling()->SetNextSibling(child);
            }
            (*it)->SetPreviousSibling(child);
            child_nodes_.insert(it, child);
        } else {
            // If index is out of range, append the child at the end
            if (!child_nodes_.empty()) {
                auto lastChild = child_nodes_.back();
                lastChild->SetNextSibling(child);
                child->SetPreviousSibling(lastChild);
            }
            child_nodes_.push_back(child);
        }
        child->SetParentNode(shared_from_this());
        TARO_LOG_DEBUG("TaroNode", "insertChildAt pid: %{public}d, cid: %{public}d", nid_, child->nid_);
    }

    void TaroNode::insertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        if (!child) return;

        detachTaroNodeReference(child);

        auto iter = std::find(child_nodes_.begin(), child_nodes_.end(), refChild);
        if (iter != child_nodes_.end()) {
            // Update the sibling pointers
            child->SetPreviousSibling((*iter)->GetPreviousSibling());
            if (child->GetPreviousSibling()) {
                child->GetPreviousSibling()->SetNextSibling(child);
            }
            child->SetNextSibling(*iter);
            (*iter)->SetPreviousSibling(child);
            child_nodes_.insert(iter, child);
        }
        child->SetParentNode(shared_from_this());
        onInsertBefore(child, refChild);
        TARO_LOG_DEBUG("TaroNode", "insertBefore pid: %{public}d, cid: %{public}d", nid_, child->nid_);
    }

    // 遍历祖先节点
    void TaroNode::TraverseAncestor(
        std::function<bool(std::shared_ptr<TaroNode>)> func) {
        try {
            auto current = this->GetParentNode();
            while (current) {
                if (func(current)) {
                    break;
                }
                current = current->GetParentNode();
            }
        } catch (const std::exception &e) {
        }
    }

    void TaroNode::ProcessRenderNodeFromTaroNode(std::shared_ptr<TaroNode> node, std::function<void(std::weak_ptr<TaroRenderNode>)> func) {
        if (is_init_) {
            std::stack<std::shared_ptr<TaroRenderNode>> stack;
            stack.push(GetHeadRenderNode());
            while (!stack.empty()) {
                auto item = stack.top();
                stack.pop();
                const auto element = item->element_ref_.lock();
                // 重绘自己节点持有的renderNode
                if (element && element == node) {
                    func(item);
                    auto childNodes = item->children_refs_;
                    for (int i = childNodes.size() - 1; i >= 0; --i) {
                        auto child = std::static_pointer_cast<TaroRenderNode>(childNodes[i]);
                        if (child) {
                            stack.push(child);
                        }
                    }
                }
            }
        }
    }

    void TaroNode::onDeleteRenderNode(std::shared_ptr<TaroNode> child) {
        // 递归卸载RenderNode
        for (auto item : child->child_nodes_) {
            if (item) {
                child->onDeleteRenderNode(item);
            }
        }
        if (!child->is_init_) return;
        if (GetHeadRenderNode() != GetFooterRenderNode()) {
            ProcessRenderNodeFromTaroNode(child, [](std::weak_ptr<TaroRenderNode> child) {
                if (auto element = child.lock()) {
                    if (auto parent = element->parent_ref_.lock()) {
                        parent->RemoveChild(element);
                    }
                }
            });
        }
        auto parent = child->GetParentNode();
        if (parent->GetFooterRenderNode()) {
            auto element = std::dynamic_pointer_cast<TaroElement>(child);
            element->event_emitter_->clearNodeEvent(element->GetNodeHandle());
            auto attribute = std::static_pointer_cast<TaroAttribute>(element);
            attribute->render_node_.reset();
            parent->GetFooterRenderNode()->RemoveChild(child->GetHeadRenderNode());
            child->SetRenderNode(nullptr);
        }
        child->is_init_ = false;
    }

} // namespace TaroDOM
} // namespace TaroRuntime
