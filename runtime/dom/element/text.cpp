//
// Created by zhutianjian on 24-4-17.
//

#include "text.h"

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/text.h"
#include "runtime/dom/document.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_click.h"
#include "runtime/dom/element/page_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroText::TaroText(napi_value node)
        : TaroElement(node) {
        tag_name_ = TAG_NAME::TEXT;
    }

    void TaroText::SetStyleContent(const std::shared_ptr<TaroRenderNode> &renderNode, const StylesheetRef &styleRef, const bool isForceUpdate) {
        renderNode->SetStyle(styleRef);
        if (isForceUpdate) {
            auto textRenderNode = std::dynamic_pointer_cast<TaroTextNode>(renderNode);
            if (textRenderNode) {
                textRenderNode->SetIsNeedUpdate(true);
            }
            renderNode->SetDrawDirty(true);
            renderNode->SetLayoutDirty(true);
        }
        renderNode->SetContent();
        if (before_pseudo_style_ || after_pseudo_style_) {
            auto childRefs = renderNode->children_refs_;
            for (auto child : childRefs) {
                if (auto textChild = std::dynamic_pointer_cast<TaroTextNode>(child)) {
                    textChild->SetStyle(styleRef);
                    textChild->SetContent();
                    break;
                }
            }
        }
    }

    void TaroText::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        std::shared_ptr<TaroNode> parentNode = GetParentNode();
        if (!parentNode) return;
        // 父节点是 Text，说明正在处理 Text 里的子节点
        if (dynamic_cast<TaroText *>(parentNode.get())) {
            if (!parentNode->HasRenderNode()) return;
            for (auto child : parentNode->child_nodes_) {
                auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(child);
                if (!item->is_init_) {
                    return;
                }
            }
            auto parentRenderNode = parentNode->GetHeadRenderNode();
            if (isPureTextNode()) {
                auto taroTextNode = std::dynamic_pointer_cast<TaroTextNode>(parentRenderNode);
                if (taroTextNode && taroTextNode->GetIsNeedUpdate()) {
                    return;
                }
                SetStyleContent(parentRenderNode, std::dynamic_pointer_cast<TaroDOM::TaroElement>(parentNode)->style_, true);
            } else if (!TaroCSSOM::TaroStylesheet::Stylesheet::compare(m_old_style, style_)) {
                SetStyleContent(parentRenderNode, std::dynamic_pointer_cast<TaroDOM::TaroElement>(parentNode)->style_, true);
            }
        } else {
            if (!HasRenderNode()) return;
            if (child_nodes_.size() > 0) {
                for (auto child : child_nodes_) {
                    auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(child);
                    if (!item->is_init_) {
                        return;
                    }
                }
            }
            auto parentElement = std::static_pointer_cast<TaroElement>(GetParentNode());
            auto textNodeStyle = GetTextNodeStyleFromElement(parentElement);
            if (isPureTextNode()) {
                if (parentElement) {
                    SetStyleContent(GetHeadRenderNode(), textNodeStyle);
                }
            } else {
                SetStyleContent(GetHeadRenderNode(), style_);
            }
        }
    }

    std::string TaroText::GetTextContent() {
        if (m_text_content.has_value()) {
            return m_text_content.value_or("");
        }
        if (isPureTextNode()) {
            return text_context_;
        } else if (node_type_ == NODE_TYPE::ELEMENT_NODE) {
            std::string textContent = text_context_;
            for (auto &iter : child_nodes_) {
                if (auto text = std::dynamic_pointer_cast<TaroText>(iter)) {
                    textContent += text->GetTextContent();
                }
            }
            return textContent;
        }
        return "";
    }

    std::string TaroText::GetTextContent(napi_value value) {
        if (!value) return "";
        NapiGetter getter(value);
        if (getter.Type() != napi_object) {
            getter = getter.GetValue();
        } else {
            getter = getter.GetProperty("textContent");
        }
        text_context_ = getter.StringOr("");
        m_text_content.reset();
        return GetTextContent();
    }

    void TaroText::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                                ATTRIBUTE_NAME name, napi_value value) {
        if (style_) {
            m_old_style = TaroCSSOM::TaroStylesheet::Stylesheet::assign(
                std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(), style_);
        }
        switch (name) {
            case ATTRIBUTE_NAME::TEXT_CONTENT:
                if (m_text_content == std::nullopt) {
                    m_text_content = GetTextContent(value);
                    UpdateContent();
                } else {
                    auto oddContext = m_text_content.value_or("");
                    auto newContent = GetTextContent(value);
                    if (oddContext != newContent) {
                        if (oddContext == "" || newContent == "") {
                            UpdateContent(true);
                        } else {
                            UpdateContent();
                        }
                        m_text_content = newContent;
                    }
                }
                break;
            default: {
                TaroElement::SetAttribute(renderNode, name, value);
            }
        }
    }

    void TaroText::RemoveAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                                   ATTRIBUTE_NAME name) {
        switch (name) {
            case ATTRIBUTE_NAME::TEXT_CONTENT:
                text_context_ = "";
                UpdateContent(true);
                break;
            default:
                TaroElement::RemoveAttribute(renderNode, name);
        }
    }

    void TaroText::Build() {
        if (!is_init_) {
            std::shared_ptr<TaroNode> parentNode = GetParentNode();
            if (parentNode != nullptr && !dynamic_cast<TaroText *>(parentNode.get())) {
                auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
                auto render_text = std::make_shared<TaroTextNode>(element);
                render_text->Build();
                if (before_pseudo_style_ || after_pseudo_style_) {
                    auto ele = std::make_shared<TaroStackNode>(element);
                    ele->Build();
                    render_text->style_ref_ = style_;
                    ele->AppendChild(render_text);
                    SetRenderNode(ele);
                } else {
                    SetRenderNode(render_text);
                }
            }
        }
    }

    void TaroText::SetTextRenderNodeInfo(const std::shared_ptr<TaroTextNode>& renderNode) {
        std::vector<std::shared_ptr<TaroNode>> vec;
        auto childNodeSize = child_nodes_.size();
        if (childNodeSize < 1) {
            if (isPureTextNode()) {
                vec = {shared_from_this()};
            }
        } else {
            vec = child_nodes_;
        }
        auto size = vec.size();
        if (size == 0) {
            renderNode->SetTextRenderNodeHasContent(false);
            return;
        }
        renderNode->SetTextRenderNodeHasContent(true);
        std::vector<std::shared_ptr<ImageInfo>> imageInfos;
        for (int32_t i = 0; i < size; i++) {
            auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(vec[i]);
            item->PreBuild();
            if (item->tag_name_ == TAG_NAME::IMAGE) {
                auto imageInfo = std::make_shared<ImageInfo>();
                imageInfo->width = -1;
                imageInfo->height = -1;
                imageInfo->nid = item->nid_;
                auto castItem = std::dynamic_pointer_cast<TaroImage>(item);
                if (castItem->attributes_->src.has_value()) {
                    if (auto src = std::get_if<std::string>(&castItem->attributes_->src.value())) {
                        imageInfo->src = *src;
                        if (imageInfo->src.starts_with("//")) {
                            imageInfo->src.insert(0, "https:");
                        }
                    } else {
                        imageInfo->src = "";
                    }
                } else {
                    imageInfo->src = "";
                }
                if (castItem->attributes_->mode.has_value()) {
                    auto it = ImageModeMap.find(castItem->attributes_->mode.value());
                    if (it != ImageModeMap.end()) {
                        imageInfo->mode = it->second;
                    } else {
                        imageInfo->mode = ScaleToFill;
                    }
                } else {
                    imageInfo->mode = ScaleToFill;
                }
                if (imageInfo->mode != ScaleToFill) {
                    renderNode->SetTextRenderNodeHasExactlyImage(false);
                }
                imageInfo->index = i;
                imageInfos.emplace_back(imageInfo);
            }
        }
        renderNode->SetTextRenderNodeImageInfos(imageInfos);
    }

    bool TaroText::Reusable(std::shared_ptr<TaroElement> &reuseElement) {
        if (!TaroElement::Reusable(reuseElement)) {
            return false;
        }
        auto textRender = std::dynamic_pointer_cast<TaroTextNode>(reuseElement->GetHeadRenderNode());
        if (!textRender || !textRender->GetTextArkNode()) {
            return false;
        }
        if (reuseElement->isTextElement()) {
            auto newParent = std::dynamic_pointer_cast<TaroDOM::TaroElement>(GetParentNode());
            if (newParent->isTextElement()) {
                return false;
            }
        }
        auto newElementChildren = child_nodes_;
        auto reuseElementChildren = reuseElement->child_nodes_;
        if (newElementChildren.size() != reuseElementChildren.size()) {
            return false;
        }
        for (int32_t i = 0; i < newElementChildren.size(); i++) {
            auto newChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(newElementChildren[i]);
            auto reuseChild = std::dynamic_pointer_cast<TaroDOM::TaroElement>(reuseElementChildren[i]);
            if (newChild->tag_name_ != reuseChild->tag_name_) {
                return false;
            }
        }
        return true;
    }

    void TaroText::Build(std::shared_ptr<TaroElement> &reuse_element) {
        auto new_node = std::dynamic_pointer_cast<TaroTextNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        std::shared_ptr<TaroNode> parentNode = GetParentNode();
        if (parentNode == nullptr || dynamic_cast<TaroText *>(parentNode.get())) {
            return;
        }
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            auto text_render = std::dynamic_pointer_cast<TaroTextNode>(reuse_element->GetHeadRenderNode());
            new_node = std::make_shared<TaroTextNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            SetRenderNode(new_node);
            new_node->SetTextRenderNodeInner(text_render->GetTextArkNode());
            new_node->SetTextRenderNodeImageNodes(text_render->GetImageArkNodeList());
            SetTextRenderNodeInfo(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->event_emitter_->clearNodeEvent(ark_handle);
            text_render->SetTextRenderNodeInner(nullptr);
            text_render->SetTextRenderNodeImageNodes({});
            text_render->SetArkUINodeHandle(nullptr);
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
        auto text_render = std::dynamic_pointer_cast<TaroTextNode>(reuse_element->GetHeadRenderNode());
        new_node->SetTextRenderNodeInner(text_render->GetTextArkNode());
        new_node->SetTextRenderNodeImageNodes(text_render->GetImageArkNodeList());
        SetTextRenderNodeInfo(new_node);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        reuse_element->event_emitter_->clearNodeEvent(ark_handle);
        text_render->SetTextRenderNodeInner(nullptr);
        text_render->SetTextRenderNodeImageNodes({});
        text_render->SetArkUINodeHandle(nullptr);
    }

    bool TaroText::bindListenEvent(const std::string &event_name) {
        if (event_name == "click") {
            auto parentNode = GetParentNode();
            if (parentNode != nullptr && dynamic_cast<TaroText *>(parentNode.get())) {
                auto innerTextRenderNode = std::dynamic_pointer_cast<TaroTextNode>(parentNode->GetHeadRenderNode());
                auto innerTextNode = innerTextRenderNode->GetTextArkNode();
                if (innerTextNode) {
                    auto nid = nid_;
                    event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name, [innerTextRenderNode, nid](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value &) -> int {
                        auto clickEvent = std::static_pointer_cast<TaroEvent::TaroEventClick>(event);
//                         auto compEvent = OH_ArkUI_NodeEvent_GetNodeComponentEvent(clickEvent->event_);
                        auto clickX = clickEvent->x_;
                        auto clickY = clickEvent->y_;
                        if (innerTextRenderNode->GetIfInTextNodeArea(nid, clickX, clickY)) {
                            return 0;
                        }
                        return -1; }, innerTextNode);
                    return true;
                }
                return false;
            }
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroText::UpdateContent(const bool isNeedReset) {
        auto parentNode = GetParentNode();
        if (!dynamic_cast<TaroText *>(parentNode.get())) {
            if (auto castText = dynamic_pointer_cast<TaroTextNode>(GetHeadRenderNode())) {
                if (isNeedReset) {
                    auto innerTextNode = castText->GetTextArkNode();
                    if (innerTextNode) {
                        event_emitter_->clearNodeEvent(innerTextNode);
                        for (auto &child : child_nodes_) {
                            std::dynamic_pointer_cast<TaroElement>(child)->getEventEmitter()->clearNodeEvent(innerTextNode);
                        }
                    }
                    castText->Reset();
                    castText->Build();
                    for (auto &child : child_nodes_) {
                        std::dynamic_pointer_cast<TaroElement>(child)->updateListenEvent();
                    }
                }
                auto parentElement = std::dynamic_pointer_cast<TaroElement>(parentNode);
                auto textNodeStyle = style_;
                if (isPureTextNode()) {
                    textNodeStyle = GetTextNodeStyleFromElement(parentElement);
                }
                castText->SetStyle(textNodeStyle);
                castText->SetIsNeedUpdate(true);
                castText->SetLayoutDirty(true);
                castText->SetDrawDirty(true);
                castText->SetContent();
            }
        } else {
            auto parent = dynamic_pointer_cast<TaroText>(parentNode);
            parent->UpdateContent(isNeedReset);
        }
    }

    void TaroText::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        std::shared_ptr<TaroPageContainer> pageContainer = TaroDocument::GetInstance()->findDOMPage(element);
        if (this->is_init_ && pageContainer && pageContainer->unstable_isTextNeedLayout) {
            UpdateContent(true);
        }
    }

    void TaroText::onAppendChild(std::shared_ptr<TaroNode> child) {
        if (this->is_init_) {
            UpdateContent(true);
        }
    }

    void TaroText::onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {
        if (this->is_init_) {
            UpdateContent(true);
        }
    }

    void TaroText::onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {
        if (this->is_init_) {
            UpdateContent(true);
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
