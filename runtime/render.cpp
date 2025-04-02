/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "render.h"

#include "NativeNodeApi.h"
#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/keyframe_vsync.h"

namespace TaroRuntime {
std::shared_ptr<TaroDOM::TaroDocument> Render::document_ = TaroDOM::TaroDocument::GetInstance();

Render::Render() {
    event_vsync_ = OH_NativeVSync_Create("EventLoop", 9);
}

Render::~Render() {
    OH_NativeVSync_Destroy(event_vsync_);
}

void Render::AttachTaroPage(uint32_t nid, std::shared_ptr<TaroDOM::TaroElement> parent) {
    Render* render = Render::GetInstance();
    std::shared_ptr<ArkUI_NodeContentHandle> component = render->GetEtsXComponent(nid);
    if (parent != nullptr && component != nullptr) {
        OH_ArkUI_NodeContent_AddNode(*component.get(), parent->GetHeadRenderNode()->GetArkUINodeHandle());
    }
}

void Render::BuildElement(std::shared_ptr<TaroDOM::TaroElement> parent) {
    std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
    stack.push(parent);

    while (!stack.empty()) {
        auto parent = stack.top();
        stack.pop();
        parent->BuildProcess();
        if (parent->lazy_node) {
            TARO_LOG_DEBUG("Render", "%{public}d component, we use lazy mode", static_cast<int>(parent->tag_name_));
            continue;
        }
        auto childNodes = parent->child_nodes_;
        for (int i = childNodes.size() - 1; i >= 0; --i) {
            auto child = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
            if (child) {
                stack.push(child);
            }
        }
    }
}

void Render::SetTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node,
                                  napi_value name, napi_value value) {
    if (node == nullptr) return;

    ArkJS arkJs(NativeNodeApi::env);
    node->SetAttribute(arkJs.getString(name), value);
}

void Render::RemoveTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node,
                                     napi_value name) {
    if (node == nullptr) return;

    ArkJS arkJs(NativeNodeApi::env);
    node->RemoveAttribute(arkJs.getString(name));
}

napi_value Render::GetTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name) {
    if (node == nullptr) return nullptr;

    ArkJS arkJs(NativeNodeApi::env);
    std::string attrName = arkJs.getString(name);
    auto attribute = node->GetAttribute(attrName);
    if (std::holds_alternative<bool>(attribute)) {
        return arkJs.createBoolean(std::get<bool>(attribute));
    } else if (std::holds_alternative<int>(attribute)) {
        return arkJs.createInt(std::get<int>(attribute));
    } else if (std::holds_alternative<std::string>(attribute)) {
        return arkJs.createString(std::get<std::string>(attribute));
    }
    return node->GetAttributeNodeValue(attrName);
}

napi_value Render::HasTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name) {
    if (node == nullptr) return nullptr;

    ArkJS arkJs(NativeNodeApi::env);
    std::string attrName = arkJs.getString(name);
    bool hasAttr = node->HasAttribute(attrName);
    return arkJs.createBoolean(hasAttr);
}

napi_value Render::HasTaroNodeAttributes(std::shared_ptr<TaroDOM::TaroElement> node) {
    if (node == nullptr) return nullptr;

    ArkJS arkJs(NativeNodeApi::env);
    bool hasAttrs = node->HasAttributes();
    return arkJs.createBoolean(hasAttrs);
}

void Render::SetEtsXComponent(uint32_t nid,
                              std::shared_ptr<ArkUI_NodeContentHandle> component) {
    if (component != nullptr && nid > 0) {
        TARO_LOG_DEBUG("Render", "nid: %{public}d", nid);
        ets_x_components_[nid] = component;
    }
}

std::shared_ptr<ArkUI_NodeContentHandle> Render::GetEtsXComponent(uint32_t nid) {
    auto iter = ets_x_components_.find(nid);
    TARO_LOG_DEBUG("Render", "x -> nid: %{public}d %{public}s", nid,
                   iter == ets_x_components_.end() ? "not found" : "found");
    if (iter == ets_x_components_.end()) {
        return nullptr;
    }
    return iter->second;
}

std::shared_ptr<TaroDOM::TaroPageContainer> Render::FindPageById(
    const std::shared_ptr<TaroDOM::TaroNode>& node,
    uint32_t nid) {
    if (node) {
        auto nodes = node.get()->child_nodes_;
        auto it = std::find_if(
            nodes.begin(), nodes.end(),
            [&](std::shared_ptr<TaroDOM::TaroNode> node) {
                auto page = std::static_pointer_cast<TaroDOM::TaroPageContainer>(node);
                return page->nid_ == nid;
            });

        if (it != nodes.end()) {
            return std::static_pointer_cast<TaroDOM::TaroPageContainer>(*it);
        }
    }
    return nullptr;
}

std::shared_ptr<TaroDOM::TaroPageContainer> Render::GetTaroPageById(
    uint32_t nid) {
    if (document_->app_ != nullptr) {
        auto page = FindPageById(document_->app_, nid);
        if (page != nullptr) {
            return page;
        }
    }
    if (document_->entry_async_ != nullptr) {
        auto page = FindPageById(document_->entry_async_, nid);
        if (page != nullptr) {
            return page;
        }
    }
    return nullptr;
}

void Render::ExecuteNodeFunc(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name, napi_value value) {
    if (node == nullptr) {
        return;
    }

    node->ExecuteFunc(name, value);
}
} // namespace TaroRuntime
