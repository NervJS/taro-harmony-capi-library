/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "element.h"

#include <stack>

#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "helper/string.h"
#include "runtime/constant.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/ark_nodes/list.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/water_flow.h"
#include "runtime/dom/element/flow_item.h"
#include "runtime/dom/element/flow_section.h"
#include "runtime/dom/element/form/group_manager.h"
#include "runtime/dom/element/list.h"
#include "runtime/dom/element/list_item_group.h"
#include "runtime/dom/element/page_container.h"
#include "runtime/dom/element/scroller_container.h"
#include "runtime/dom/element/water_flow.h"
#include "runtime/dom/event/event_visible.h"

namespace TaroRuntime {
namespace TaroDOM {
    std::unordered_map<std::string, std::shared_ptr<TaroContext>> TaroContext::context_mapping_; // 路由和路由对应的页面上下文的映射

    std::shared_ptr<TaroContext> TaroContext::GetContext(std::shared_ptr<TaroPageContainer> pageElement) {
        auto iter = context_mapping_.find(pageElement->id_);
        if (iter != context_mapping_.end()) {
            return iter->second;
        } else {
            std::shared_ptr<TaroContext> context = std::make_shared<TaroContext>(pageElement);
            context_mapping_.insert({pageElement->id_, context});
            return context;
        }
    }

    std::shared_ptr<TaroContext> TaroContext::CloneContext(std::shared_ptr<TaroContext> originContext, std::shared_ptr<TaroElement> measureRoot) {
        std::shared_ptr<TaroContext> context = std::make_shared<TaroContext>(originContext->page_element_.lock());
        context->group_manger_ = originContext->group_manger_;
        context->measure_root_ = measureRoot;
        return context;
    }

    TaroContext::TaroContext(std::shared_ptr<TaroPageContainer> pageElement)
        : page_element_(pageElement),
          page_id_(pageElement->id_),
          measure_root_(pageElement),
          page_path_(string::extractPathWithoutParams(pageElement->id_)) {}

    TaroContext::~TaroContext() {
        context_mapping_.erase(page_id_);
        if (group_manger_) {
            group_manger_->Clear();
        }
    }

    // Element构造函数
    TaroElement::TaroElement(napi_value node)
        : TaroAttribute(node),
          TaroElementEvent(this) {
        tag_name_ = GetTagName(node);
#if IS_DEBUG
        TARO_LOG_DEBUG("TaroElement", "构建 nid_: %{public}d; tagName: %{public}d; nodeType: %{public}d; className: %{public}s", nid_, tag_name_, node_type_, class_name_.c_str());
#endif
    }

    // Element 析构
    TaroElement::~TaroElement() {
        event_emitter_ = nullptr;
#if IS_DEBUG
        TARO_LOG_DEBUG("TaroElement", "析构 nid_: %{public}d; nodeType: %{public}d className: %{public}s", nid_, node_type_, class_name_.c_str());
#endif
        // 修复Fixed节点的render node可能不析构
        auto renderNode = GetHeadRenderNode();
        if (renderNode && style_->position.has_value() && style_->position.value() == PropertyType::Position::Fixed) {
            if (auto fixedRoot = renderNode->parent_ref_.lock()) {
                fixedRoot->RemoveChild(renderNode);
            }
        }
        after_render_node_ = nullptr;
        before_render_node_ = nullptr;
    }

    void TaroElement::GetNodeAttributes() {
        TaroAttribute::GetNodeAttributes();

        ArkJS arkJs(NativeNodeApi::env);
        class_list_.bindElement(std::static_pointer_cast<TaroElement>(shared_from_this()));
        class_list_.reset(class_name_);
        // 表单元素
        if (isFormElement()) {
            disabled_ = GetDisabledAttribute(GetAttributeNodeValue("disabled"));
        }
        is_inline_ = false;
    }

    void TaroElement::SetAttributesToRenderNode() {
        TaroAttribute::SetAttributesToRenderNode(GetHeadRenderNode());

        Disabled::SetValueToNode(disabled_);
    }

    std::variant<bool, int, std::string, StylesheetRef, napi_value>
    TaroElement::GetAttribute(std::string name) {
        return GetAttribute(GetAttributeName(name), GetAttributeNodeValue(name));
    }

    std::variant<bool, int, std::string, StylesheetRef, napi_value>
    TaroElement::GetAttribute(ATTRIBUTE_NAME name, napi_value value) {
        switch (name) {
            case ATTRIBUTE_NAME::CLASS:
                return class_list_.value();
            default:
                return TaroAttribute::GetAttribute(name, value);
        }
    }

    void TaroElement::SetHeadRenderNode(std::shared_ptr<TaroRenderNode> head_render_node) {
        auto odd_header_render = GetHeadRenderNode();
        if (is_init_ && odd_header_render) {
            // Note: 取消原 head_render_node 与父级关联
            std::shared_ptr<TaroNode> parent = GetParentNode();
            if (parent != nullptr && parent->is_init_) {
                auto parent_render = parent->GetFooterRenderNode();
                if (parent_render) {
                    parent_render->RemoveChild(odd_header_render);
                }
            }
        }
        TaroNode::SetHeadRenderNode(head_render_node);
    }

    void TaroElement::SetFooterRenderNode(
        std::shared_ptr<TaroRenderNode> footer_render_node) {
        if (is_init_) {
            // Note: 取消原 footer_render_node 与子节点关联
            for (const auto& child : child_nodes_) {
                if (child && child->is_init_) {
                    auto child_header = child->GetHeadRenderNode();
                    if (child_header) {
                        child_header->RemoveChild(GetFooterRenderNode());
                    }
                }
            }
        }
        TaroNode::SetFooterRenderNode(footer_render_node);
    }

    void TaroElement::HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) {
        switch (name) {
            case ATTRIBUTE_NAME::CLASS: {
                class_list_.reset(curValue);
                TARO_LOG_DEBUG("TaroElement", "className: %{public}s", class_list_.value().c_str());
                break;
            };
            case ATTRIBUTE_NAME::STYLE: {
                if (!is_init_)
                    return;
                invalidateStyle(TaroCSSOM::Validity::ElementInvalid);
                break;
            };
            case ATTRIBUTE_NAME::ANIMATION: {
                if (!is_init_)
                    return;
                createJSAnimation();
                break;
            };
            default:
                break;
        }
    }

    // 获取节点类型
    TAG_NAME TaroElement::GetTagName(napi_value node) {
        std::string str = NapiGetter::GetProperty(node, "tagName").StringOr("");
        return GetTagName(str);
    }

    TAG_NAME TaroElement::GetTagName(std::string str) {
        TAG_NAME tagName = TAG_NAME::UNKNOWN;
        auto it = TAG_NAME_MAP.find(str);
        if (it != TAG_NAME_MAP.end()) {
            tagName = it->second;
        }

        return tagName;
    }

    PROPERTY_NAME TaroElement::GetPropertyName(napi_value node) {
        std::string str = NapiGetter::GetValue(node).StringOr("");
        PROPERTY_NAME propertyName = PROPERTY_NAME::UNKNOWN;
        auto it = PROPERTY_NAME_MAP.find(str);
        if (it != PROPERTY_NAME_MAP.end()) {
            propertyName = it->second;
        }

        return propertyName;
    }

    int TaroElement::GetFuncCode(napi_value node) {
        std::string str = NapiGetter::GetValue(node).StringOr("");
        return GetFuncCode(str);
    }

    int TaroElement::GetFuncCode(std::string str) {
        FUNC_CODE funcCode = FUNC_CODE::UNKNOWN;
        auto it = FUNC_CODE_MAP.find(str);
        if (it != FUNC_CODE_MAP.end()) {
            funcCode = it->second;
        }
        return static_cast<int>(funcCode);
    }

    void TaroElement::invalidateStyleInternal() {
        invalidateStyle(TaroCSSOM::Validity::ElementInvalid);
    }

    void TaroElement::invalidateStyle(TaroCSSOM::Validity validity) {
        // 标“脏”
        if (validity != TaroCSSOM::Validity::Valid) {
            SetStateFlag(STATE_FLAG::IS_COMPUTED_STYLE_INVALID_FLAG);
            if (validity == TaroCSSOM::Validity::SubtreeInvalid) {
                SetStateFlag(STATE_FLAG::DESCENDANT_NEED_STYLE_RESOLUTION);
            }
        }
        // 找到最近的一颗脏子树，向祖先冒上去
        markAncestorsForInvalidatedStyle();
        // 在下一个vsync进行重计算处理
        if (!context_)
            return;
        DirtyTaskPipeline::GetInstance()->AddDirtyStyleNode(context_->page_element_.lock());
    }

    // 向祖先进行标脏，标记到最小脏子树为止
    void TaroElement::markAncestorsForInvalidatedStyle() {
        TraverseAncestor([](std::shared_ptr<TaroNode> parentNode) {
            auto parentElement =
                std::static_pointer_cast<TaroElement>(parentNode);
            if (parentElement->HasStateFlag(
                    STATE_FLAG::DESCENDANT_NEED_STYLE_RESOLUTION)) {
                return true;
            }
            parentElement->SetStateFlag(
                STATE_FLAG::DESCENDANT_NEED_STYLE_RESOLUTION);
            return false;
        });
    }

    void TaroElement::SetIsInline(bool isInline) {
        is_inline_ = isInline;
    }

    bool TaroElement::GetIsInline() {
        return is_inline_;
    }

    void TaroElement::BuildProcess() {
        if (!TaroNode::ignore_pre_build) {
            PreBuild();
        } else {
            TaroNode::ignore_pre_build = false;
        }

        if (!TaroNode::ignore_build) {
            Build();
        } else {
            TaroNode::ignore_build = false;
        }

        if (!TaroNode::ignore_post_build) {
            PostBuild();
        } else {
            TaroNode::ignore_post_build = false;
        }
    }

    void TaroElement::createContext() {
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        // 绑定pageRoot
        auto parentNode = element->GetParentNode();
        if (parentNode) {
            auto parentElement = std::static_pointer_cast<TaroElement>(parentNode);
            if (parentElement->context_) {
                if (
                    element->tag_name_ == TAG_NAME::FLOW_ITEM ||
                    element->tag_name_ == TAG_NAME::LIST_ITEM ||
                    element->tag_name_ == TAG_NAME::STICKY_SECTION ||
                    element->tag_name_ == TAG_NAME::SWIPER_ITEM) {
                    // context 需要创建一个新的
                    if (element->tag_name_ == TAG_NAME::LIST_ITEM && std::dynamic_pointer_cast<TaroListItemGroup>(parentElement)) {
                        context_ = parentElement->context_; // group就已经创建了，继承group即可
                    } else {
                        context_ = TaroContext::CloneContext(parentElement->context_, element);
                    }
                } else {
                    // 从父节点继承传递下来
                    context_ = parentElement->context_;
                }
            } else {
                auto pageRoot = getPageRoot();
                if (pageRoot) {
                    context_ = TaroContext::GetContext(std::static_pointer_cast<TaroPageContainer>(pageRoot));
                } else {
                    TARO_LOG_FATAL("TaroElement", "pageRoot is nullptr tagName: %{public}d nid: %{public}d", tag_name_, nid_);
                }
            }
        }
    }

    void TaroElement::createContextRecursively(std::shared_ptr<TaroContext> context) {
        auto root = std::static_pointer_cast<TaroElement>(shared_from_this());
        std::stack<std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>> stack;
        stack.push(root);
        if (!root)
            return;
        while (!stack.empty()) {
            auto item = stack.top();
            stack.pop();
            if (root == item) {
                root->context_ = context;
            } else {
                item->createContext();
            }
            auto childNodes = item->child_nodes_;
            for (int i = childNodes.size() - 1; i >= 0; --i) {
                auto child = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(childNodes[i]);
                if (child) {
                    stack.push(child);
                }
            }
        }

        if (auto renderNode = root->GetHeadRenderNode()) {
            // 有可能被标脏了，导致无法再次标上
            renderNode->SetLayoutDirty(false);
            renderNode->SetLayoutDirty(true);
        }
    }

    std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> TaroElement::GetTextNodeStyleFromElement(const std::shared_ptr<TaroElement>& element) {
        auto textNodeStyle = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        if (element && element->is_init_) {
            // 从父节点继承下来
            textNodeStyle->color = element->style_->color;
            textNodeStyle->fontStyle = element->style_->fontStyle;
            textNodeStyle->fontSize = element->style_->fontSize;
            textNodeStyle->fontWeight = element->style_->fontWeight;
            textNodeStyle->fontFamily = element->style_->fontFamily;
            textNodeStyle->textDecorationColor = element->style_->textDecorationColor;
            textNodeStyle->textDecorationStyle = element->style_->textDecorationStyle;
            textNodeStyle->textDecorationLine = element->style_->textDecorationLine;

            textNodeStyle->textAlign = element->style_->textAlign;
            textNodeStyle->textOverflow = element->style_->textOverflow;
            textNodeStyle->verticalAlign = element->style_->verticalAlign;
            textNodeStyle->webkitLineClamp = element->style_->webkitLineClamp;
            textNodeStyle->lineHeight = element->style_->lineHeight;
            textNodeStyle->letterSpacing = element->style_->letterSpacing;
            textNodeStyle->whiteSpace = element->style_->whiteSpace;
            textNodeStyle->wordBreak = element->style_->wordBreak;
            textNodeStyle->overflow = element->style_->overflow;
            textNodeStyle->display = element->style_->display;
            textNodeStyle->boxOrient = element->style_->boxOrient;
        }
        return textNodeStyle;
    }

    void TaroElement::PreBuild() {
        if (!is_init_) {
            GetNodeAttributes();
            createContext();
            // 样式匹配
            MatchStylesheet();
        }
    }

    const std::shared_ptr<TaroNode> TaroElement::getParentNode(TAG_NAME tagName) {
        std::shared_ptr<TaroNode> node = shared_from_this();
        while (node->GetParentNode() != nullptr) {
            auto tempElement =
                std::dynamic_pointer_cast<TaroDOM::TaroElement>(node->GetParentNode());
            if (tempElement->tag_name_ == tagName) {
                return tempElement;
            }
            node = node->GetParentNode();
        }
        return nullptr;
    }

    void TaroElement::MatchStylesheet() {
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        // 样式匹配
        if (context_) {
            auto rules_path = TaroCSSOM::CSSStyleSheet::GetInstance()->getStylePagePath(context_->page_path_);
            auto new_class_style = TaroCSSOM::CSSStyleSheet::GetInstance()->getStylesheetByClassNames(element, element->class_list_, rules_path);
            if (inline_style_ == nullptr) {
                style_ = new_class_style.stylesheet;
            } else {
                style_ = TaroCSSOM::TaroStylesheet::Stylesheet::assign(new_class_style.stylesheet, inline_style_);
            }

            before_pseudo_style_ = new_class_style.before_stylesheet;
            after_pseudo_style_ = new_class_style.after_stylesheet;
        } else {
            style_ = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        }
    }

    TaroElementRef GetPreviousStaticSibling(TaroElement* taroElementRef) {
        std::shared_ptr<TaroNode> prevNode = taroElementRef->GetPreviousSibling();
        auto prevElement = std::static_pointer_cast<TaroElement>(prevNode);
        while (prevElement && prevElement->IsFixed()) {
            prevElement = std::static_pointer_cast<TaroElement>(prevElement->GetPreviousSibling());
        }
        return prevElement;
    }

    TaroElementRef GetNextStaticSibling(TaroElement* taroElementRef) {
        std::shared_ptr<TaroNode> nextNode = taroElementRef->GetNextSibling();
        auto nextElement = std::static_pointer_cast<TaroElement>(nextNode);
        while (nextElement && nextElement->IsFixed()) {
            nextElement = std::static_pointer_cast<TaroElement>(nextElement->GetNextSibling());
        }
        return nextElement;
    }

    void TaroElement::AppendOrInsertIntoParent() {
        if (this->tag_name_ == TAG_NAME::FLOW_SECTION) {
            return;
        }
        auto parent = GetParentNode();
        auto parentNode = parent->GetFooterRenderNode();
        if (parentNode == nullptr)
            return;

        TaroElementRef nextElement = GetNextStaticSibling(this);
        TaroElementRef prevElement = GetPreviousStaticSibling(this);

        const auto parentElement = std::static_pointer_cast<TaroElement>(parent);

        auto header = GetHeadRenderNode();
        if (prevElement != nullptr && prevElement->is_init_) {
            parentNode->InsertChildAfter(header, prevElement->GetHeadRenderNode());
        } else if (nextElement != nullptr && nextElement->is_init_) {
            parentNode->InsertChildBefore(header, nextElement->GetHeadRenderNode());
        } else if (parentElement->before_render_node_) {
            parentNode->InsertChildAfter(header, parentElement->before_render_node_);
        } else if (parentElement->after_render_node_) {
            parentNode->InsertChildBefore(header, parentElement->after_render_node_);
        } else {
            parentNode->AppendChild(header);
        }
    }

    void TaroElement::PostBuild() {
        if (!is_init_) {
            if (style_->position.value_or(PropertyType::Position::Static) !=
                PropertyType::Position::Fixed) {
                auto parent = GetParentNode();
                if (parent != nullptr && parent->is_init_) {
                    // Note: 确定节点在父节点中的位置，挂载到父节点的RenderNode上
                    AppendOrInsertIntoParent();
                }
            }
            createJSAnimation();
            if (info_.registerd && !info_.registerdToArk) {
                registerVisibleEvent();
                info_.registerdToArk = true;
            }
            is_init_ = true;
            updateListenEvent(); // Note: 文字事件会在 init 后更新
        }
        SetAttributesToRenderNode();
        // Fixed节点
        MountFixedRenderNode();
        // 伪元素创建/销毁
        MountPseudoRenderNode();
    }

    void TaroElement::MountPseudoRenderNode() {
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto footer = GetFooterRenderNode();

        if (!footer)
            return;

        if (before_render_node_) {
            if (!before_pseudo_style_) {
                footer->RemoveChild(before_render_node_);
                before_render_node_ = nullptr;
            } else {
                before_render_node_->SetStyle(before_pseudo_style_);
            }
        } else if (!before_render_node_ && before_pseudo_style_) {
            before_render_node_ = std::make_shared<TaroStackNode>(element);
            before_render_node_->Build();
            footer->InsertChildAt(before_render_node_, 0);
            before_render_node_->SetStyle(before_pseudo_style_);
        }
        if (after_render_node_) {
            if (!after_pseudo_style_) {
                footer->RemoveChild(after_render_node_);
                after_render_node_ = nullptr;
            } else {
                after_render_node_->SetStyle(after_pseudo_style_);
            }
        } else if (!after_render_node_ && after_pseudo_style_) {
            after_render_node_ = std::make_shared<TaroStackNode>(element);
            after_render_node_->Build();
            footer->AppendChild(after_render_node_);
            after_render_node_->SetStyle(after_pseudo_style_);
        }
    }

    void TaroElement::MountFixedRenderNode() {
        auto header = GetHeadRenderNode();
        if (!header || !header->style_ref_)
            return;

        auto new_position = header->style_ref_->position.value_or(PropertyType::Position::Static);
        if (new_position == PropertyType::Position::Fixed && !header->old_style_ref_) {
            const auto page = getPageRoot();
            if (page) {
                const auto pageElement = std::static_pointer_cast<TaroDOM::TaroPageContainer>(page);
                pageElement->getFixedRoot()->AppendChild(header);
                header->AddNotifyFixedRoot();
                createContextRecursively(page->context_);
                return;
            }
        } else if (header->old_style_ref_) {
            auto old_position = header->old_style_ref_->position.value_or(PropertyType::Position::Static);
            if (old_position != new_position) {
                const auto page = getPageRoot();
                if (page) {
                    const auto pageElement = std::static_pointer_cast<TaroDOM::TaroPageContainer>(page);
                    if (old_position == PropertyType::Position::Fixed) {
                        pageElement->getFixedRoot()->RemoveChild(header);
                        // 还原回去原本的位置
                        AppendOrInsertIntoParent();
                        if (auto parent = std::static_pointer_cast<TaroElement>(GetParentNode())) {
                            createContextRecursively(parent->context_);
                        }
                    } else if (new_position == PropertyType::Position::Fixed) {
                        // 从正常文档流卸掉
                        const auto parent = GetParentNode();
                        if (parent) {
                            parent->GetFooterRenderNode()->RemoveChild(header);
                            pageElement->getFixedRoot()->AppendChild(header);
                            header->AddNotifyFixedRoot();
                            createContextRecursively(page->context_);
                        }
                    }
                    return;
                }
            }
        }
    }

    bool TaroElement::IsFixed() {
        if (!style_)
            return false;

        return style_->position.value_or(PropertyType::Position::Static) ==
               PropertyType::Position::Fixed;
    }

    // 检查祖先是否显隐
    bool TaroElement::CheckAncestorsVisibility() {
        auto current = std::static_pointer_cast<TaroElement>(shared_from_this());
        while (current) {
            if (current->style_) {
                if (
                    current->style_->display.has_value() && current->style_->display.value() == PropertyType::Display::None ||
                    current->style_->visibility.has_value() && current->style_->visibility.value() == ArkUI_Visibility::ARKUI_VISIBILITY_HIDDEN) {
                    return false;
                }
            }
            current = std::static_pointer_cast<TaroElement>(current->GetParentNode());
        }
        return true;
    }

    const std::shared_ptr<TaroElement> TaroElement::getPageRoot() {
        if (context_) {
            auto pageElement = context_->page_element_.lock();
            if (pageElement)
                return pageElement;
        }
        auto parentElement = std::static_pointer_cast<TaroElement>(shared_from_this());
        while (parentElement) {
            if (parentElement->tag_name_ == TAG_NAME::PAGE) {
                return parentElement;
            }
            parentElement = std::static_pointer_cast<TaroElement>(
                parentElement->GetParentNode());
        }
        return nullptr;
    }

    void TaroElement::updateListenEvent() {
        if (!is_init_)
            return;

        if (!ignore_update_event) {
            for (auto& pair : listeners_ref_) {
                bindListenEvent(pair.first);
            }
        } else {
            ignore_update_event = false;
        }
    }

    void TaroElement::createJSAnimation() {
        auto header = GetHeadRenderNode();
        if (header == nullptr || js_animation_ == nullptr) {
            return;
        }
        header->createJsAnimation(js_animation_);
    }

    /**
     * 寻找离自己最近的块级父节点（用于文本节点）
     * 目前认为，只要不是文本节点，其他都是块级节点
     */
    std::shared_ptr<TaroElement> TaroElement::findClosedBlockParent() {
        auto parent = std::static_pointer_cast<TaroElement>(shared_from_this());
        do {
            parent = std::static_pointer_cast<TaroElement>(parent->GetParentNode());
        } while (parent && parent->node_type_ == NODE_TYPE::TEXT_NODE || parent->tag_name_ == TAG_NAME::TEXT);
        return parent;
    }

    napi_value TaroElement::ExecuteFunc(napi_value name, napi_value params) {
        if (this && render_node_ != nullptr) {
            return ExecuteFunc(render_node_, name, params);
        }
        return nullptr;
    }

    napi_value TaroElement::ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params) {
        ArkJS arkJs(NativeNodeApi::env);
        std::string funcName = NapiGetter::GetValue(name).StringOr("");
        FUNC_CODE code = static_cast<FUNC_CODE>(GetFuncCode(funcName));
        NapiGetter paramsGetter(params);
        switch (code) {
            case FUNC_CODE::CLASSLIST_ADD: {
                std::vector<NapiGetter> params = paramsGetter.Vector().value();
                std::vector<std::string> tokens;
                for (auto& param : params) {
                    tokens.push_back(param.StringOr(""));
                }
                class_list_.add(tokens);
                return nullptr;
            }
            case FUNC_CODE::CLASSLIST_REMOVE: {
                std::vector<NapiGetter> params = paramsGetter.Vector().value();
                std::vector<std::string> tokens;
                for (auto& param : params) {
                    tokens.push_back(param.StringOr(""));
                }
                class_list_.remove(tokens);
                return nullptr;
            }
            case FUNC_CODE::CLASSLIST_CONTAINS: {
                int isContains = class_list_.contains(paramsGetter.StringOr(""));
                return arkJs.createBoolean(isContains == 1);
            }
            case FUNC_CODE::CLASSLIST_TOGGLE: {
                std::vector<NapiGetter> params = paramsGetter.Vector().value();
                auto token = params[0].StringOr("");
                auto force = params[1].Bool();
                int isToggle = 0;
                if (force.has_value()) {
                    isToggle = class_list_.toggle(token, force.value_or(false));
                } else {
                    isToggle = class_list_.toggle(token);
                }
                return arkJs.createBoolean(isToggle == 1);
            }
            case FUNC_CODE::CLASSLIST_REPLACE: {
                std::vector<NapiGetter> params = paramsGetter.Vector().value();
                auto oldToken = params[0].StringOr("");
                auto newToken = params[1].StringOr("");
                class_list_.replace(oldToken, newToken);
                return nullptr;
            }
            case FUNC_CODE::CLASSLIST_TO_STRING: {
                return arkJs.createString(class_list_.value());
            }
            case FUNC_CODE::CLASSLIST_LENGTH: {
                return arkJs.createInt(class_list_.length());
            }
            case FUNC_CODE::UNKNOWN:
                TARO_LOG_ERROR("TaroElement", "ExecuteFunc: unknown func: %{public}s", funcName.c_str());
        }

        return nullptr;
    }

    // TODO需要定义成纯虚函数，由各元素重写复用逻辑
    void TaroElement::Build(std::shared_ptr<TaroElement>& reuse_element) {
        TARO_LOG_DEBUG("TaroNodeAdapter", "tag name %{public}d, we will implement it latter", tag_name_);
    }

    bool TaroElement::Reusable(std::shared_ptr<TaroElement>& reuse_element) {
        if (reuse_element->tag_name_ == tag_name_ && reuse_element->GetNodeHandle()) {
            return true;
        }
        return false;
    }

    void TaroElement::registerVisibleEvent() {
        // nextTick 再监听可见性，确保节点大小和位置都已经 layout 完毕
        std::weak_ptr<TaroNode> node_ref = shared_from_this();
        DirtyTaskPipeline::GetInstance()->RegistryNextTick([node_ref] {
            if (auto node = node_ref.lock()) {
                std::static_pointer_cast<TaroElement>(node)->registerOnScrollVisibleChangeEvent();
            }
        });
    }

    void TaroElement::registerOnScrollVisibleChangeEvent() {
        auto element = shared_from_this();
        auto current = element;
        while (current) {
            if (auto scroll_container = std::dynamic_pointer_cast<TaroScrollerContainer>(current)) {
                scroll_container->on_visible_node.emplace(std::static_pointer_cast<TaroElement>(element));
                if (!scroll_container->is_visible_node_listener_registered) {
                    std::weak_ptr<TaroScrollerContainer> weak_scroll_container = scroll_container;
                    auto scroll_fun = [weak_scroll_container](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                        auto scroll_container = weak_scroll_container.lock();

                        if (!scroll_container)
                            return 0;

                        scroll_container->handleVisibleNode();

                        return 0;
                    };
                    if (auto water_flow = std::dynamic_pointer_cast<TaroWaterFlow>(scroll_container)) {
                        element->lazy_child_item = true;
                        scroll_container->event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_DID_WATER_FLOW_ON_SCROLL, "scrollVisible", scroll_fun);
                    } else if (auto list = std::dynamic_pointer_cast<TaroList>(scroll_container)) {
                        element->lazy_child_item = true;
                        scroll_container->event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_DID_LIST_ON_SCROLL, "scrollVisible", scroll_fun);
                    } else {
                        scroll_container->event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_DID_SCROLL_ON_SCROLL, "scrollVisible", scroll_fun);
                    }
                    scroll_container->is_visible_node_listener_registered = true;
                }
            }

            current = current->GetParentNode();
        }

        handleVisibilityInfo();
    }

    void TaroElement::handleVisibilityInfo() {
        auto visible_element = std::static_pointer_cast<TaroElement>(shared_from_this());
        VisibilityInfo visibilityInfo = VisibleEventListener::GetInstance()->CalculateNodeVisibility(visible_element, visible_element->info_);
        float intersectionRatio = visibilityInfo.intersectionRatio;
        bool isIntersectionActive = VisibleEventListener::GetInstance()->checkThresholds(visible_element->info_.triggeredThresholds_, intersectionRatio);
        if (intersectionRatio >= 0 && isIntersectionActive) {
            VisibleEventListener::GetInstance()->callVisibilityFunc(visible_element->info_, visibilityInfo);
        }
    }

    void TaroElement::bindVisibleEvent(CallbackInfo& info) {
        info_ = info;
        info_.registerd = true;
        if (is_init_) {
            registerVisibleEvent();
            info_.registerdToArk = true;
        }
    }

    void TaroElement::unbindVisibleEvent() {
        if (info_.registerd && info_.registerdToArk) {
            info_.registerd = false;
            info_.registerdToArk = false;
            info_.triggeredThresholds_.clear();
            if (info_.callback_ref) {
                napi_delete_reference(NativeNodeApi::env, info_.callback_ref);
                info_.callback_ref = nullptr;
            }
        }
    }

    void TaroElement::CheckMatchEmptySelector() {
        if (GetMatchEmptySelector()) {
            invalidateStyle(TaroCSSOM::Validity::ElementInvalid);
        }
    }

    bool TaroElement::GetMatchEmptySelector() {
        return has_empty_selector_;
    }

    void TaroElement::SetMatchEmptySelector(bool val) {
        has_empty_selector_ = val;
    }

    bool TaroElement::checkIsInScrollContainerIndex(bool useCacheCount) {
        int32_t cacheCount = 0;
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        int32_t index = -1;
        auto parent_node = element->GetParentNode();
        if (!parent_node) {
            return false;
        }

        if (parent_node->lazy_node) {
            if (auto list = std::dynamic_pointer_cast<TaroList>(parent_node)) {
                if (useCacheCount) {
                    cacheCount = list->GetCacheCount();
                }
                if (auto list_node = std::dynamic_pointer_cast<TaroListNode>(list->GetHeadRenderNode())) {
                    // TODO: 需要抽离 TaroLazyScrollContainer
                    index = list_node->adapter_->getTaroNodeIndexInDataSource(element);
                    uint32_t startIndex = 0;
                    uint32_t dataSize = list_node->adapter_->getDataSize();
                    uint32_t endIndex = dataSize - 1;
                    if (list->start_index > cacheCount) {
                        startIndex = list->start_index - cacheCount;
                    }
                    if (list->end_index + cacheCount < dataSize) {
                        endIndex = list->end_index + cacheCount;
                    }
                    if (startIndex > index || endIndex < index) {
                        return false;
                    }
                }
            } else if (auto section = std::dynamic_pointer_cast<TaroFlowSection>(parent_node)) {
                auto grand_parent_node = parent_node->GetParentNode();
                if (auto water_flow = std::dynamic_pointer_cast<TaroWaterFlow>(grand_parent_node)) {
                    if (useCacheCount) {
                        cacheCount = water_flow->GetCacheCount();
                    }
                    if (auto water_flow_node = std::dynamic_pointer_cast<TaroWaterFlowNode>(water_flow->GetHeadRenderNode())) {
                        index = water_flow_node->adapter_->getTaroNodeIndexInDataSource(element);
                        uint32_t startIndex = 0;
                        uint32_t dataSize = water_flow_node->adapter_->getDataSize();
                        uint32_t endIndex = dataSize - 1;
                        if (water_flow->start_index > cacheCount) {
                            startIndex = water_flow->start_index - cacheCount;
                        }
                        if (water_flow->end_index + cacheCount < dataSize) {
                            endIndex = water_flow->end_index + cacheCount;
                        }
                        if (startIndex > index || endIndex < index) {
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
