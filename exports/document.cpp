//
// Created by zhutianjian on 24-8-19.
//

#include "document.h"

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "helper/TaroAllocationMetrics.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "helper/api_cost_statistic.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/api/selectory_query.h"
#include "runtime/render.h"

namespace TaroHarmonyLibrary {
const std::vector<napi_property_descriptor> Document::desc = {
    {"buildTaroNode", nullptr, Document::BuildTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"createTaroNode", nullptr, Document::CreateTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"appendTaroNode", nullptr, Document::AppendTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"removeTaroNode", nullptr, Document::RemoveTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"replaceTaroNode", nullptr, Document::ReplaceTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"insertBeforeTaroNode", nullptr, Document::InsertBeforeTaroNode, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"setTaroNodeAttribute", nullptr, Document::SetTaroNodeAttribute, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"removeTaroNodeAttribute", nullptr, Document::RemoveTaroNodeAttribute, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodeAttribute", nullptr, Document::GetTaroNodeAttribute, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"hasTaroNodeAttribute", nullptr, Document::HasTaroNodeAttribute, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"hasTaroNodeAttributes", nullptr, Document::HasTaroNodeAttributes, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodeProperty", nullptr, Document::GetTaroNodeProperty, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodeChildAt", nullptr, Document::GetTaroNodeChildAt, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodeById", nullptr, Document::GetTaroNodeById, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodesByTagName", nullptr, Document::GetTaroNodesByTagName, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getTaroNodesByClassName", nullptr, Document::GetTaroNodesByClassName, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"querySelectDOM", nullptr, Document::QuerySelectDOM, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"querySelectDOMSync", nullptr, Document::QuerySelectDOMSync, nullptr, nullptr, nullptr, napi_default, nullptr},
};

napi_value Document::BuildTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    napi_value node_content = args[0];
    napi_value nid_value = args[1];
    ArkUI_NodeContentHandle nodeContentHandle_ = nullptr;
    OH_ArkUI_GetNodeContentFromNapiValue(env, node_content, &nodeContentHandle_);
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::NapiGetter::GetValue(nid_value).Int32Or(0);

    if (nodeContentHandle_ == nullptr) {
        return nullptr;
    }

    render->SetEtsXComponent(nid, std::make_shared<ArkUI_NodeContentHandle>(nodeContentHandle_));
    render->AttachTaroPage(nid, render->GetTaroPageById(nid));

    return nullptr;
}

napi_value Document::CreateTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    TaroRuntime::NODE_TYPE nodeType = TaroRuntime::TaroDOM::TaroNode::GetNodeType(args[0]);
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    if (nodeType == TaroRuntime::NODE_TYPE::DOCUMENT_NODE) {
        render->document_->SetNodeValue(args[0]);
    }
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> el = render->document_->GetElementById(nid);
    if (el == nullptr) {
        el = render->document_->CreateElement(args[0]);
    }

    TARO_LOG_DEBUG("Manager", "nid: %{public}d nodeType: %{public}d nodeName: %{public}s", el->nid_, el->node_type_, el->GetNodeName().c_str());
    return nullptr;
}

napi_value Document::AppendTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> parent = render->document_->GetElementById(nid);
    int32_t childNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[1]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> child = render->document_->GetElementById(childNid);

    if (parent == nullptr) {
        TARO_LOG_FATAL("Manager", "nid: %{public}d is null", nid);
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(folly::sformat("AppendTaroNode Error parentNid: {} is null", nid)));
#else
        return nullptr;
#endif
    }
    if (child == nullptr) {
        TARO_LOG_FATAL("Manager", "childNid: %{public}d is null", childNid);
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(folly::sformat("AppendTaroNode Error childNid: {} is null", childNid)));
#else
        return nullptr;
#endif
    }

    if (parent->tag_name_ == TaroRuntime::TAG_NAME::APP || parent->tag_name_ == TaroRuntime::TAG_NAME::ENTRY_ASYNC) {
        TARO_LOG_INFO("Manager", "app init.");
        parent->appendChild(child);
        render->BuildElement(child);
        render->AttachTaroPage(child->nid_, child);
    } else if (render->document_->isPlaceholderNode(parent)) {
        parent->appendChild(child);
    } else {
        parent->appendChild(child);
        if (parent->is_init_ && !parent->lazy_node) {
            render->BuildElement(child);
            TARO_LOG_DEBUG("Manager", "BUILD parent: %{public}d %{public}s child: %{public}d %{public}s",
                           parent->nid_, parent->class_name_.c_str(), child->nid_, child->class_name_.c_str());
        }
    }
    auto parentEle = std::static_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(parent);
    parentEle->CheckMatchEmptySelector();

    TARO_LOG_DEBUG("Manager", "parentNid %{public}d", parent->nid_);
    return nullptr;
}

napi_value Document::RemoveTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> parent = render->document_->GetElementById(nid);
    int32_t childNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[1]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> child = render->document_->GetElementById(childNid);

    if (parent == nullptr) {
        TARO_LOG_ERROR("Manager", "nid: %{public}d is null", nid);
        // 父节点可能会在事件回调过程中被从Document移除
        return nullptr;
    }

    TARO_LOG_DEBUG("Manager", "removeChild parent-nid: %{public}d nid: %{public}d", parent->nid_, childNid);
    if (child != nullptr) {
        parent->removeChild(child);
        if (parent->is_init_) {
            if (auto header = parent->GetHeadRenderNode()) {
                header->SetLayoutDirty(true);
            }
        }
    }
    auto parentEle = std::static_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(parent);
    parentEle->CheckMatchEmptySelector();

    return nullptr;
}

napi_value Document::ReplaceTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> parent = render->document_->GetElementById(nid);

    if (parent == nullptr) {
        TARO_LOG_FATAL("Manager", "nid: %{public}d is null", nid);
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(folly::sformat("ReplaceTaroNode Error parentNid: {} is null", nid)));
#else
        return nullptr;
#endif
    }

    if (parent != nullptr) {
        int32_t newChildNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[1]);
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> newChild = render->document_->GetElementById(newChildNid);
        int32_t oldChildNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[2]);
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> oldChild = render->document_->GetElementById(oldChildNid);
        if (oldChild != nullptr && newChild != nullptr) {
            parent->replaceChild(newChild, oldChild);
            if (parent->is_init_ && !parent->lazy_node) {
                render->BuildElement(newChild);
            }
        }
    }
    auto parentEle = std::static_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(parent);
    parentEle->CheckMatchEmptySelector();

    return nullptr;
}

napi_value Document::InsertBeforeTaroNode(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> parent = render->document_->GetElementById(nid);

    if (parent == nullptr) {
        TARO_LOG_FATAL("Manager", "nid: %{public}d is null", nid);
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(folly::sformat("InsertBeforeTaroNode Error parentNid: {} is null", nid)));
#else
        return nullptr;
#endif
    }

    if (parent != nullptr) {
        int32_t newChildNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[1]);
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> newChild = render->document_->GetElementById(newChildNid);
        int32_t refChildNid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[2]);
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> refChild = render->document_->GetElementById(refChildNid);
        TARO_LOG_DEBUG("Manager", "insertBefore parent-nid: %{public}d ref-nid: %{public}d nid: %{public}d",
                       parent->nid_, refChildNid, newChildNid);
        if (refChild != nullptr && newChild != nullptr) {
            parent->insertBefore(newChild, refChild);
            if (parent->is_init_ && !parent->lazy_node) {
                render->BuildElement(newChild);
            }
        }
    }
    auto parentEle = std::static_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(parent);
    parentEle->CheckMatchEmptySelector();

    TARO_PRINT_MEMORY();
    return nullptr;
}

napi_value Document::SetTaroNodeAttribute(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    render->SetTaroNodeAttribute(node, args[1], args[2]);

    TARO_PRINT_MEMORY();
    return nullptr;
}

napi_value Document::RemoveTaroNodeAttribute(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    render->RemoveTaroNodeAttribute(node, args[1]);

    TARO_PRINT_MEMORY();
    return nullptr;
}

napi_value Document::GetTaroNodeAttribute(napi_env env, napi_callback_info info) {
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    return render->GetTaroNodeAttribute(node, args[1]);
}

napi_value Document::HasTaroNodeAttribute(napi_env env, napi_callback_info info) {
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    return render->HasTaroNodeAttribute(node, args[1]);
}

napi_value Document::HasTaroNodeAttributes(napi_env env, napi_callback_info info) {
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    return render->HasTaroNodeAttributes(node);
}

napi_value Document::GetTaroNodeById(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }
    std::string id = arkJs.getString(args[1]);
    auto dom = TaroRuntime::SelectorQuery::GetInstance()->getElementById(node, id);
    if (dom) {
        return dom->GetNodeValue();
    }
    return nullptr;
}

napi_value Document::GetTaroNodesByTagName(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }
    std::string tag_name = arkJs.getString(args[1]);
    auto dom_list = TaroRuntime::SelectorQuery::GetInstance()->getElementsByTagName(node, tag_name);
    napi_value array = arkJs.createArray();
    size_t i = 0;
    for (const auto& item : dom_list) {
        if (auto el = item.lock()) {
            arkJs.setArrayElement(array, i++, el->GetNodeValue());
        }
    }
    return array;
}

napi_value Document::GetTaroNodesByClassName(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }
    std::string class_name = arkJs.getString(args[1]);
    auto dom_list = TaroRuntime::SelectorQuery::GetInstance()->getElementsByClassName(node, class_name);
    napi_value array = arkJs.createArray();
    size_t i = 0;
    for (const auto& item : dom_list) {
        if (auto el = item.lock()) {
            arkJs.setArrayElement(array, i++, el->GetNodeValue());
        }
    }
    return array;
}

napi_value Document::QuerySelectDOM(napi_env env, napi_callback_info info) {
    TIMER_US_FUNCTION();
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 4);

    napi_value dom_callback_value = args[3];

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        node = render->document_;
    }
    std::string selector = arkJs.getString(args[1]);
    bool is_select_all = arkJs.getBoolean(args[2]);
    napi_valuetype type = arkJs.getType(dom_callback_value);
    if (type != napi_function) {
        TARO_LOG_ERROR("Manager", "Invalid callback function");
        return nullptr;
    }

    NapiHandleScopeWrapper wrapper(env);
    napi_ref callback_ref = arkJs.createReference(dom_callback_value);

    auto dom_list = TaroRuntime::SelectorQuery::GetInstance()->querySelector(node, selector, is_select_all);
    napi_value array = arkJs.createArray();
    size_t i = 0;
    for (const auto& item : dom_list) {
        if (auto el = item.lock()) {
            arkJs.setArrayElement(array, i++, el->GetNodeValue());
        }
    }
    arkJs.call(arkJs.getReferenceValue(callback_ref), {array});
    arkJs.deleteReference(callback_ref);
    return nullptr;
}

napi_value Document::QuerySelectDOMSync(napi_env env, napi_callback_info info) {
    TIMER_US_FUNCTION();
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Document", "nid[%{public}d] is not found", nid);
        node = render->document_;
    }
    std::string selector = arkJs.getString(args[1]);
    bool is_select_all = arkJs.getBoolean(args[2]);

    NapiHandleScopeWrapper wrapper(env);
    auto dom_list = TaroRuntime::SelectorQuery::GetInstance()->querySelector(node, selector, is_select_all);
    napi_value array = arkJs.createArray();
    size_t i = 0;
    for (const auto& item : dom_list) {
        if (auto el = item.lock()) {
            arkJs.setArrayElement(array, i++, el->GetNodeValue());
        }
    }
    return array;
}

napi_value Document::GetTaroNodeProperty(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);

    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        // 节点会在事件回调过程中被从Document移除
        return nullptr;
    }

    TaroRuntime::PROPERTY_NAME property_name = TaroRuntime::TaroDOM::TaroElement::GetPropertyName(args[1]);
    TimeCostStatistic::instance()->record(int(property_name));
    switch (property_name) {
        case TaroRuntime::PROPERTY_NAME::PARENT_ELEMENT: {
            auto parent = node->GetParentNode();
            if (parent && parent->node_type_ == TaroRuntime::NODE_TYPE::ELEMENT_NODE) {
                return parent->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::PARENT_NODE: {
            if (auto parent = node->GetParentNode()) {
                return parent->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::CHILD_NODES: {
            auto childs = node->child_nodes_;
            napi_value child_array;
            napi_create_array(env, &child_array);
            for (size_t i = 0; i < childs.size(); i++) {
                napi_value child_value = childs[i]->GetNodeValue();
                napi_set_element(env, child_array, i, child_value);
            }
            return child_array;
        }
        case TaroRuntime::PROPERTY_NAME::CHILDREN: {
            auto childs = node->child_nodes_;
            napi_value child_array;
            napi_create_array(env, &child_array);
            for (size_t i = 0; i < childs.size(); i++) {
                if (childs[i]->node_type_ == TaroRuntime::NODE_TYPE::ELEMENT_NODE) {
                    napi_value child_value = childs[i]->GetNodeValue();
                    napi_set_element(env, child_array, i, child_value);
                }
            }
            return child_array;
        }
        case TaroRuntime::PROPERTY_NAME::PREVIOUS_SIBLING: {
            if (auto previous_sibling = node->GetPreviousSibling()) {
                return previous_sibling->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::NEXT_SIBLING: {
            if (auto next_sibling = node->GetNextSibling()) {
                return next_sibling->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::FIRST_CHILD: {
            if (node->child_nodes_.size() > 0) {
                return node->child_nodes_[0]->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::FIRST_ELEMENT_CHILD: {
            for (auto child : node->child_nodes_) {
                if (child->node_type_ == TaroRuntime::NODE_TYPE::ELEMENT_NODE) {
                    return child->GetNodeValue();
                }
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::LAST_CHILD: {
            if (node->child_nodes_.size() > 0) {
                return node->child_nodes_[node->child_nodes_.size() - 1]->GetNodeValue();
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::LAST_ELEMENT_CHILD: {
            for (int i = node->child_nodes_.size() - 1; i >= 0; i--) {
                if (node->child_nodes_[i]->node_type_ == TaroRuntime::NODE_TYPE::ELEMENT_NODE) {
                    return node->child_nodes_[i]->GetNodeValue();
                }
            }
            break;
        }
        case TaroRuntime::PROPERTY_NAME::DATASET: {
            return node->GetDataset();
        }
        default:
            break;
    }

    return nullptr;
}

napi_value Document::GetTaroNodeChildAt(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);

    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(folly::sformat("GetTaroNodeChildAt Error nid: {} is not found", nid)));
#else
        return nullptr;
#endif
    }

    int32_t index = TaroRuntime::NapiGetter::GetValue(args[1]).Int32Or(0);
    if (index < 0 || index >= node->child_nodes_.size()) {
        return nullptr;
    }
    auto child = node->child_nodes_[index];
    return child->GetNodeValue();
}
} // namespace TaroHarmonyLibrary
