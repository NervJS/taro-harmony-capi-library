//
// Created on 2024/7/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "flow_section.h"

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/water_flow.h"

namespace TaroRuntime {
namespace TaroDOM {

    TaroFlowSection::TaroFlowSection(napi_value node)
        : TaroElement(node), column_(1) {}

    TaroFlowSection::~TaroFlowSection() {}

    void TaroFlowSection::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto parent = GetParentNode();
            if (parent != nullptr) {
                auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
                auto index_ = std::distance(parent->child_nodes_.begin(), it);

                int index = 0;
                auto childNodes1 = parent->child_nodes_;
                for (int i = 0; i < childNodes1.size(); ++i) {
                    if (i == index_) {
                        break;
                    }
                    index += childNodes1[i]->child_nodes_.size();
                }

                auto childNodes = element->child_nodes_;
                for (int i = 0; i < childNodes.size(); ++i) {
                    if (childNodes[i]) {
                        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
                        water_flow->adapter_->insertItemAt(childNodes[i], index + i);
                    }
                }
                SetRenderNode(parent->GetHeadRenderNode());
            }
            lazy_node = true;
        }
    }

    void TaroFlowSection::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        switch (name) {
            case ATTRIBUTE_NAME::COLUMN:
                GetColumnAttribute(value);
                if (is_init_) {
                    SetColumnAttribute();
                }
                break;
            case ATTRIBUTE_NAME::ROW_GAP:
                GetRowGapAttribute(value);
                if (is_init_) {
                    SetRowGapAttribute();
                }
                break;
            case ATTRIBUTE_NAME::COLUMN_GAP:
                GetColumnGapAttribute(value);
                if (is_init_) {
                    SetColumnGapAttribute();
                }
                break;
            case ATTRIBUTE_NAME::MARGIN:
                GetMarginAttribute(value);
                if (is_init_) {
                    SetMarginAttribute();
                }
                break;
            default:
                break;
        }
    }

    void TaroFlowSection::GetColumnAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<int32_t> res = getter.Int32();
        if (res.has_value()) {
            column_ = res.value();
        }
    }

    void TaroFlowSection::GetRowGapAttribute(const napi_value& value) {
        double gap = GetDoubleAttributeValue(value);
        rowGap_.set(gap);
    }

    void TaroFlowSection::GetColumnGapAttribute(const napi_value& value) {
        double gap = GetDoubleAttributeValue(value);
        columnGap_.set(gap);
    }

    void TaroFlowSection::GetMarginAttribute(const napi_value& value) {
        NapiGetter getter(value);
    
        double marginTop = GetDoubleAttributeValue(getter.GetProperty("marginTop").GetNapiValue());
        double marginRight = GetDoubleAttributeValue(getter.GetProperty("marginRight").GetNapiValue());
        double marginBottom = GetDoubleAttributeValue(getter.GetProperty("marginBottom").GetNapiValue());
        double marginLeft = GetDoubleAttributeValue(getter.GetProperty("marginLeft").GetNapiValue());

        margin_.set(SectionMargin({marginTop, marginRight, marginBottom, marginLeft}));
    }

    double TaroFlowSection::GetDoubleAttributeValue(const napi_value& value) {
        NapiGetter getter(value);
        double double_value = 0.0;
        if (getter.String().has_value()) {
            auto dimensionContext = DimensionContext::GetInstance();
            if (context_) {
                dimensionContext = TaroCSSOM::CSSStyleSheet::GetInstance()->GetCurrentPageDimension(context_->page_path_);
            }
            double_value = Dimension::FromString(getter.String().value()).ConvertToVp(dimensionContext);
        } else if (getter.Double().has_value()) {
            double_value = getter.Double().value();
        }
    
        return double_value;
    }

    void TaroFlowSection::SetColumnAttribute() {
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            water_flow->updateSectionColumn(index_, column_);
        }
    }

    void TaroFlowSection::SetRowGapAttribute() {
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            water_flow->updateRowGap(index_, vp2Px(rowGap_.value()));
        }
    }

    void TaroFlowSection::SetColumnGapAttribute() {
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            water_flow->updateColumnGap(index_, vp2Px(columnGap_.value()));
        }
    }

    void TaroFlowSection::SetMarginAttribute() {
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            water_flow->updateMargin(
                index_,
                vp2Px(margin_.value().marginTop),
                vp2Px(margin_.value().marginRight),
                vp2Px(margin_.value().marginBottom),
                vp2Px(margin_.value().marginLeft)
            );
        }
    }

    void TaroFlowSection::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            child->SetParentNode(nullptr);
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);
            if (water_flow->adapter_->removeItem(child) == 0) {
                // update section
                water_flow->updateSectionCount(index_, -1);
            }
            TaroElement::onRemoveChild(child, is_detach_render_node);
        }
    }

    void TaroFlowSection::onAppendChild(std::shared_ptr<TaroNode> child) {
        TaroElement::onAppendChild(child);
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);

            int index = 0;
            auto childNodes = parent->child_nodes_;
            for (int i = 0; i < childNodes.size(); ++i) {
                index += childNodes[i]->child_nodes_.size();
                if (i == index_) {
                    break;
                }
            }
            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            if (water_flow->adapter_->insertItemAt(child, index - 1) == 0) {
                // update section
                water_flow->updateSectionCount(index_, 1);
            }
        }
    }

    void TaroFlowSection::onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {
        TaroElement::onReplaceChild(newChild, oldChild);

        auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(GetParentNode()->GetHeadRenderNode());
        if (water_flow != nullptr) {
            water_flow->adapter_->reloadItem(newChild, oldChild);
        }
    }

    void TaroFlowSection::onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        TaroElement::onInsertBefore(child, refChild);
        auto parent = GetParentNode();
        if (parent != nullptr && parent->is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto it = find(parent->child_nodes_.begin(), parent->child_nodes_.end(), element);
            auto index_ = std::distance(parent->child_nodes_.begin(), it);

            auto water_flow = std::static_pointer_cast<TaroWaterFlowNode>(parent->GetHeadRenderNode());
            water_flow->adapter_->insertItemBefore(child, refChild);
            // update section
            water_flow->updateSectionCount(index_, 1);
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime