/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "list_item_group.h"

#include "runtime/dom/ark_nodes/list_item_group.h"

namespace TaroRuntime {
namespace TaroDOM {

    TaroListItemGroup::TaroListItemGroup(napi_value node)
        : TaroElement(node) {}

    TaroListItemGroup::~TaroListItemGroup() {}

    void TaroListItemGroup::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_item_group = std::make_shared<TaroListItemGroupNode>(element);
            render_item_group->Build();
            render_item_group->SetShouldPosition(false);
            SetRenderNode(render_item_group);
        }
    }

    void TaroListItemGroup::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        SetSpace();
        auto render_item_group = std::static_pointer_cast<TaroListItemGroupNode>(GetHeadRenderNode());
        render_item_group->SetStyle(style_);
    }

    void TaroListItemGroup::GetSpace(const napi_value &value) {
        NapiGetter getter(value);
        TaroHelper::Optional<double> val = getter.Double();
        if (val.has_value()) {
            space.set(val.value());
        }
    }

    void TaroListItemGroup::SetSpace() {
        if (space.has_value()) {
            std::shared_ptr<TaroListItemGroupNode> render = std::static_pointer_cast<TaroListItemGroupNode>(GetHeadRenderNode());
            render->setSpace(space.value());
        }
    }

    void TaroListItemGroup::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::SPACE:
                GetSpace(value);
                if (is_init_) {
                    SetSpace();
                }
                break;
            default:
                break;
        }
    }

    void TaroListItemGroup::setStickyHeader(std::shared_ptr<TaroRenderNode> renderNode) {
        if (renderNode->GetArkUINodeHandle()) {
            std::shared_ptr<TaroListItemGroupNode> render = std::static_pointer_cast<TaroListItemGroupNode>(GetHeadRenderNode());
            render->setHeader(renderNode->GetArkUINodeHandle());
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime