/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "StyleInvalidator.h"

#include "runtime/dom/node_visitor.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    // 标注节点样式更新
    void Invalidator::markElementUpdateStyle(const std::shared_ptr<TaroDOM::TaroElement>& el,
                                             MatchElement matchElement) {
        switch (matchElement) {
            // 仅更新当前节点样式
            case MatchElement::Subject:
            case MatchElement::Multiple: {
                invalidateIfNeeded(el);
                break;
            }
            // 更新直接子节点
            case MatchElement::Parent: {
                for (auto& childNode : el->child_nodes_) {
                    auto childElement = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNode);
                    invalidateIfNeeded(childElement);
                }
                break;
            }
            // 遍历更新子节点
            case MatchElement::Ancestor: {
                invalidateStyleForDescendants(el);
                break;
            }
        }
    }

    bool Invalidator::invalidateIfNeeded(const std::shared_ptr<TaroDOM::TaroElement>& el) {
        el->invalidateStyleInternal();
        // 子树失效，无需向下标记
        if (el->HasStateFlag(STATE_FLAG::IS_SUBTREE_COMPUTED_STYLE_INVALID_FLAG)) {
            return false;
        }
        return true;
    }

    void Invalidator::invalidateStyleForDescendants(const std::shared_ptr<TaroDOM::TaroElement>& root) {
        // 更新当前子节点
        invalidateIfNeeded(root);
        // 标记子树失效
        root->SetStateFlag(STATE_FLAG::IS_SUBTREE_COMPUTED_STYLE_INVALID_FLAG);

        // 遍历子树，全部失效
        TaroDOM::ElementVisitor visitor(
            [&](const std::shared_ptr<TaroDOM::TaroElement>& descendant) {
                // 对子节点进行操作
                if (invalidateIfNeeded(descendant) == false) {
                    // 子树失效，无需向下标记
                    return TaroDOM::VisitResult::Skip;
                }
                return TaroDOM::VisitResult::Continue;
            });
        visitor.traverse(std::static_pointer_cast<TaroDOM::TaroNode>(root));
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
