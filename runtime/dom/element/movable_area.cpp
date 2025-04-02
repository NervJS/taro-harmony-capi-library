/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "movable_area.h"

#include "runtime/dom/ark_nodes/stack.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroMovableArea::TaroMovableArea(napi_value node)
        : TaroElement(node) {}

    void TaroMovableArea::Build() {
        if (!is_init_) {
            auto element =
                std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_movable_area = std::make_shared<TaroStackNode>(element);
            render_movable_area->Build();
            SetRenderNode(render_movable_area);
        }
    }

    void TaroMovableArea::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        GetHeadRenderNode()->SetStyle(style_);
    }

    void TaroMovableArea::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                                       ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::SCALE_AREA: {
                auto scaleArea = getter.BoolNull();
                if (scaleArea.has_value()) {
                    scale_area = scaleArea.value();
                }
                break;
            }
            default:
                break;
        }
    }

    void TaroMovableArea::Build(std::shared_ptr<TaroElement> &reuse_element) {
        std::shared_ptr<TaroStackNode> new_node = std::dynamic_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            new_node = std::make_shared<TaroStackNode>(element);
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

} // namespace TaroDOM
} // namespace TaroRuntime