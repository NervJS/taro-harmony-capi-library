/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "list_item.h"

#include "runtime/dom/ark_nodes/list_item.h"

namespace TaroRuntime {
namespace TaroDOM {

    TaroListItem::TaroListItem(napi_value node)
        : TaroElement(node) {}

    TaroListItem::~TaroListItem() {}

    void TaroListItem::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_list_item = std::make_shared<TaroListItemNode>(element);
            render_list_item->Build();
            SetRenderNode(render_list_item);
        }
    }

    void TaroListItem::Build(std::shared_ptr<TaroElement>& reuse_element) {
        std::shared_ptr<TaroListItemNode> new_node = std::dynamic_pointer_cast<TaroListItemNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            new_node = std::make_shared<TaroListItemNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            SetRenderNode(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
            return;
        }
        if (ark_handle == nullptr) {
            auto parent = new_node->parent_ref_.lock();
            if (parent) {
                new_node->Build();
                parent->UpdateChild(new_node);
                new_node->ClearDifferOldStyleFromElement();
            }
            return;
        }
        new_node->SetArkUINodeHandle(ark_handle);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
    }

    void TaroListItem::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto render_list_item = std::static_pointer_cast<TaroListItemNode>(GetHeadRenderNode());
        render_list_item->SetStyle(style_);
    }

    void TaroListItem::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
    }

} // namespace TaroDOM
} // namespace TaroRuntime
