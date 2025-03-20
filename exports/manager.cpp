/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "manager.h"

#include "arkjs/ArkJS.h"
#include "helper/FileManager.h"
#include "helper/ImageLoader.h"
#include "helper/SwitchManager.h"
#include "helper/TaroAllocationMetrics.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "helper/api_cost_statistic.h"
#include "runtime/NapiSetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/font/FontFamilyManager.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/animation/trigger/frame_trigger_master.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "runtime/dom/element/scroll_view.h"
#include "runtime/dom/element/swiper.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_hm/event_task_thread.h"
#include "runtime/dom/event/event_visible.h"
#include "runtime/render.h"
#include "runtime/tmp_resource_manager.h"
#include "system/system_info.h"

namespace TaroHarmonyLibrary {
const std::vector<napi_property_descriptor> Manager::desc = {
    {"initJSCAPIBridge", nullptr, Manager::InitJSCAPIBridge, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"onXComponentStateChange", nullptr, Manager::OnXComponentStateChange, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"onEntryAbilityEvent", nullptr, Manager::OnEntryAbilityEvent, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getComputedStyle", nullptr, Manager::GetComputedStyle, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"registerEtsBuilder", nullptr, Manager::RegisterEtsBuilder, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"initStylesheet", nullptr, Manager::InitStylesheet, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"UpdateStylesheet", nullptr, Manager::UpdateStylesheet, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"UpdateEnvRule", nullptr, Manager::UpdateEnvRule, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"createObserve", nullptr, Manager::CreateObserve, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"disconnectObserve", nullptr, Manager::DisconnectObserve, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"registerResource", nullptr, Manager::RegisterResource, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getCurrentOffset", nullptr, Manager::GetCurrentOffset, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"scrollTo", nullptr, Manager::ScrollTo, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"updatePageSync", nullptr, Manager::UpdatePageSync, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"loadImage", nullptr, Manager::LoadImage, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"registryNextTick", nullptr, Manager::NextTick, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"registerFontFamily", nullptr, Manager::RegisterFontFamily, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"initFileManger", nullptr, Manager::InitFileManger, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"executeNodeFunc", nullptr, Manager::ExecuteNodeFunc, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"updateDimensionContext", nullptr, Manager::UpdateDimensionContext, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"unstable_SetPageIsTextNeedLayout", nullptr, Manager::Unstable_SetPageIsTextNeedLayout, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"unstable_unsafe_SetSwiperOverflow", nullptr, Manager::Unstable_unsafe_SetSwiperOverflow, nullptr, nullptr, nullptr, napi_default, nullptr}};

std::unordered_map<std::string, ArkUI_DrawableDescriptor*> Manager::resourceMap_;

static ArkUI_NativeNodeAPI_1* nodeAPI;

Manager::~Manager() {
    TARO_LOG_INFO("Manager", "~Manager");
    for (auto iter = resourceMap_.begin(); iter != resourceMap_.end(); ++iter) {
        if (iter->second != nullptr) {
            OH_ArkUI_DrawableDescriptor_Dispose(iter->second);
            iter->second = nullptr;
        }
    }
}

napi_value Manager::InitJSCAPIBridge(napi_env env, napi_callback_info info) {
    API_FUNC_COST();

    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    TaroRuntime::TaroDOM::TaroEvent::EventHelper::instance()->init(args[1]);
    TaroSystem::SystemInfo::instance()->initDeviceInfo(args[1]);
    TARO_LOG_INFO("Manager", "%{public}s thread:%{public}d", "init ok", pthread_self());
    TaroRuntime::TaroDOM::TaroEvent::TaroEventTaskThread::instance()->init();

    return nullptr;
}

napi_value Manager::UpdateDimensionContext(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    TaroRuntime::NativeNodeApi::env = env;
    TaroRuntime::TaroCSSOM::CSSStyleSheet::GetInstance()->InitConfig(args[0]);
    return nullptr;
}

napi_value Manager::InitStylesheet(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::NativeNodeApi::env = env;
    // stylesheet，页面移除需要移除；需要支持参数启用缓存
    TaroRuntime::TaroCSSOM::CSSStyleSheet::GetInstance()->Init(args[0], args[1], args[2]);
    return nullptr;
}

napi_value Manager::UpdateEnvRule(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    
    TaroRuntime::NativeNodeApi::env = env;
    auto inst = TaroRuntime::TaroCSSOM::CSSStyleSheet::GetInstance();
    inst->InitConfig(args[1]);
    inst->UpdateCurrentPageDimension(TaroRuntime::NapiGetter::GetValue(args[0]).StringOr(""));
    
    int32_t nid = TaroRuntime::NapiGetter::GetValue(args[2]).Int32Or(0);
     TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    auto root = render->GetTaroPageById(nid);

    std::stack<std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>> stack;
    stack.push(root);
    if (!root) return nullptr;
    while (!stack.empty()) {
        auto parent = stack.top();
        stack.pop();
        
        bool shouleMatch = false;
        for (auto rule: parent->rule_list_) {
            if (rule->has_env) {
                shouleMatch = true;
                break;
            }
        }
        if (shouleMatch) {
            parent->MatchStylesheet();
            // parent->BuildProcess();
            parent->PreBuild();
            if (parent->is_reused_ && parent->GetNodeHandle() == nullptr) {
                parent->Build(parent);
            } else {
                parent->Build();
            }
            parent->PostBuild();
        }
        auto childNodes = parent->child_nodes_;
        for (int i = childNodes.size() - 1; i >= 0; --i) {
            auto child = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(childNodes[i]);
            if (child) {
                stack.push(child);
            }
        }
    }
    return nullptr;
}

napi_value Manager::UpdateStylesheet(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 4);

    TaroRuntime::NativeNodeApi::env = env;
    // stylesheet，页面移除需要移除；需要支持参数启用缓存
    TaroRuntime::TaroCSSOM::CSSStyleSheet::GetInstance()->Init(args[0], args[1], args[2]);

    napi_value nid_value = args[3];

    int32_t nid = TaroRuntime::NapiGetter::GetValue(nid_value).Int32Or(0);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    auto root = render->GetTaroPageById(nid);

    std::stack<std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>> stack;
    stack.push(root);
    if (!root) return nullptr;
    while (!stack.empty()) {
        auto parent = stack.top();
        stack.pop();
        parent->MatchStylesheet();
        // parent->BuildProcess();
        parent->PreBuild();
        if (parent->is_reused_ && parent->GetNodeHandle() == nullptr) {
            parent->Build(parent);
        } else {
            parent->Build();
        }
        parent->PostBuild();
        // 折叠后需要重新判断可见性，如果是虚拟列表的子节点，则不走这里判断，会通过 index 的变化触发可见性判断
        if (parent->info_.registerd && parent->info_.registerdToArk && !parent->lazy_child_item) {
            std::weak_ptr<TaroRuntime::TaroDOM::TaroElement> element_ref = parent;
            TaroRuntime::DirtyTaskPipeline::GetInstance()->RegistryNextTick([element_ref] {
                if (auto element = element_ref.lock()) {
                    std::static_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(element)->handleVisibilityInfo();
                }
            });
        }
        auto childNodes = parent->child_nodes_;
        bool is_lazy_parent = parent->lazy_node;
        for (int i = childNodes.size() - 1; i >= 0; --i) {
            auto child = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroElement>(childNodes[i]);
            if (child) {
                // Fixme 下面的逻辑暂时注释掉，因为不在屏的元素不matchstyle在所有元素高度都改变的时候会导致抖动，另外，小p fixme
                //                 if (is_lazy_parent) {
                //                     if (!child->checkIsInScrollContainerIndex(true)) {
                //                         child->is_need_match_style = true;
                //                         continue;
                //                     }
                //                 }
                stack.push(child);
            }
        }
    }
    return nullptr;
}

napi_value Manager::OnEntryAbilityEvent(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    // Note: create/destroy/foreground/background/continue/new-want/dump/save-state/share/back-pressed
    std::string eventName = TaroRuntime::NapiGetter::GetValue(args[0]).StringOr("");
    TARO_LOG_DEBUG("Manger", " ability event:%{public}s", eventName.c_str());
    if (eventName == "destroy") {
        TaroRuntime::NativeNodeApi::getInstance()->setDestroy(true);
        TaroRuntime::DirtyTaskPipeline::GetInstance()->CleanAllTasks();
        TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
        render->document_->DetachAllTaroNode();
        // 清除临时文件缓存
    }
    return nullptr;
}

napi_value Manager::OnXComponentStateChange(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    int32_t nid = TaroRuntime::NapiGetter::GetValue(args[0]).Int32Or(0);
    TaroRuntime::X_COMPONENT_STAGE stage = static_cast<TaroRuntime::X_COMPONENT_STAGE>(TaroRuntime::NapiGetter::GetValue(args[1]).Int32Or(0));

    bool isActive = stage == TaroRuntime::X_COMPONENT_STAGE::LOAD || stage == TaroRuntime::X_COMPONENT_STAGE::SHOW;
    TaroRuntime::TaroAnimate::TaroFrameTriggerMaster::instance()->changeXComponentState(nid, isActive);

    if (stage == TaroRuntime::X_COMPONENT_STAGE::UNLOAD) {
        TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> parent;
        if (render->FindPageById(render->document_->app_, nid)) {
            parent = render->document_->app_;
        } else if (render->FindPageById(render->document_->entry_async_, nid)) {
            parent = render->document_->entry_async_;
        }

        if (parent == nullptr) {
            TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found in app.", nid);
            return nullptr;
        }
        std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
        if (parent != nullptr && node != nullptr) {
            parent->removeChild(node);
        }
    }
    TARO_LOG_DEBUG("Manager", "nid: %{public}d; stage: %{public}s", nid, stage == TaroRuntime::X_COMPONENT_STAGE::SHOW ? "show" : "hide");
    TARO_PRINT_MEMORY();
    return nullptr;
}

napi_value Manager::GetComputedStyle(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found.", nid);
        return nullptr;
    }
    auto render_node = node ? node->GetHeadRenderNode() : nullptr;
    if (!node->is_init_ || render_node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not init.", nid);
        return nullptr;
    }
    std::string name = TaroRuntime::NapiGetter::GetValue(args[1]).StringOr("");
    napi_value result;
    float value = render_node->GetComputedStyle(name.c_str());
    napi_status ret_status = napi_create_double(env, value, &result);
    if (ret_status != napi_ok) {
        napi_throw_error(env, nullptr, "Failed to create double value");
        return nullptr;
    }

    return result;
}

napi_value Manager::RegisterEtsBuilder(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    napi_ref callback;
    napi_create_reference(env, args[0], 1, &callback);
    std::string routerStr = TaroRuntime::NapiGetter::GetValue(args[1]).StringOr("");
    TaroRuntime::NativeNodeApi::getInstance()->RegisterBuilderCallback(std::move(routerStr), callback, env);
    return nullptr;
}

napi_value Manager::CreateObserve(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 5);

    napi_value node_value = args[0];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> element = render->document_->GetElementById(nid);
    if (element == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }

    napi_value margin = args[1];
    float marginLeft = arkJs.getDouble(arkJs.getObjectProperty(margin, "left"));
    float marginTop = arkJs.getDouble(arkJs.getObjectProperty(margin, "top"));
    float marginRight = arkJs.getDouble(arkJs.getObjectProperty(margin, "right"));
    float marginBottom = arkJs.getDouble(arkJs.getObjectProperty(margin, "bottom"));

    // 获取第三个参数 thresholds
    napi_value thresholds_value = args[2];
    bool is_array;
    napi_is_array(env, thresholds_value, &is_array);
    if (!is_array) {
        TARO_LOG_ERROR("Manager", "Invalid thresholds parameter");
        return nullptr;
    }
    std::unordered_map<float, bool> thresholds;
    uint32_t length = arkJs.getArrayLength(thresholds_value);
    for (uint32_t i = 0; i < length; ++i) {
        napi_value element = arkJs.getArrayElement(thresholds_value, i);
        napi_valuetype element_type;
        napi_typeof(env, element, &element_type);
        if (element_type != napi_number) {
            TARO_LOG_ERROR("Manager", "Array elements must be numbers");
            return nullptr;
        }

        double value = arkJs.getDouble(element);
        thresholds[static_cast<float>(value)] = false; // 初始化为 false
    }

    if (length == 0) {
        thresholds[0.0f] = false; // 初始化为 false
    }

    // 获取第四个参数 initialRatio
    napi_value initialRatio_value = args[3];
    napi_valuetype initialRatio_type;
    napi_typeof(env, initialRatio_value, &initialRatio_type);
    if (initialRatio_type != napi_number) {
        TARO_LOG_ERROR("Manager", "Invalid initialRatio parameter");
        return nullptr;
    }
    double initial_ratio_double = arkJs.getDouble(initialRatio_value);
    float initialRatio = static_cast<float>(initial_ratio_double);

    // 获取第五个参数 callback
    napi_value callback = args[4];
    napi_valuetype valuetype;
    napi_typeof(env, callback, &valuetype);
    if (valuetype != napi_function) {
        TARO_LOG_ERROR("Manager", "Invalid callback function");
        return nullptr;
    }
    napi_ref callback_ref = nullptr;
    napi_create_reference(env, callback, 1, &callback_ref);

    // 启动监听
    VisibleEventListener::GetInstance()->Register(
        element, {marginLeft, marginTop, marginRight, marginBottom}, thresholds, initialRatio, callback_ref);
    return nullptr;
}

napi_value Manager::DisconnectObserve(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> element = render->document_->GetElementById(nid);
    if (element == nullptr) {
        TARO_LOG_WARN("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }

    VisibleEventListener::GetInstance()->Disconnect(element);
    return nullptr;
}

napi_value Manager::RegisterResource(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    size_t strLength;
    napi_status status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLength);
    if (status != napi_ok) {
        return nullptr;
    }
    std::string str_value(strLength, '\0');
    status = napi_get_value_string_utf8(env, args[0], &str_value[0], strLength + 1, &strLength);
    if (status != napi_ok) {
        return nullptr;
    }
    auto iter = resourceMap_.find(str_value);
    if (iter != resourceMap_.end()) {
        TARO_LOG_WARN("Manager", "%s{public}s has already been registered.", str_value.c_str());
        OH_ArkUI_DrawableDescriptor_Dispose(iter->second);
        iter->second = nullptr;
        resourceMap_.erase(iter);
    }
    ArkUI_DrawableDescriptor* drawableDescriptor;
    int32_t res = OH_ArkUI_GetDrawableDescriptorFromResourceNapiValue(env, args[1], &drawableDescriptor);
    if (res != 0) {
        TARO_LOG_ERROR("Manager", "Register resource failed:%{public}d", res);
        return nullptr;
    }
    resourceMap_.insert_or_assign(str_value, drawableDescriptor);
    return nullptr;
}

napi_value Manager::UpdatePageSync(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);
    napi_value node_value = args[0];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found.", nid);
        return nullptr;
    }
    auto render_node = node ? node->GetHeadRenderNode() : nullptr;
    if (!node->is_init_ || render_node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not init.", nid);
        return nullptr;
    }
    auto root = render_node->GetRootRenderNode();
    if (!root) {
        TARO_LOG_ERROR("Manager", "root is not found");
        return nullptr;
    }
    TaroRuntime::DirtyTaskPipeline::GetInstance()->FlushUpdateTasks();
    return nullptr;
}

napi_value Manager::GetCurrentOffset(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);
    napi_value node_value = args[0];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr || !node->is_init_) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found or not init.", nid);
        return nullptr;
    }
    auto scroller_node = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroScrollView>(node);
    if (scroller_node == nullptr) {
        TARO_LOG_ERROR("Manager", "node[%{public}d] is not a scroll-view", nid);
        return nullptr;
    }

    TaroRuntime::TaroDOM::OffsetResult result = scroller_node->GetCurrentOffset();
    napi_value scroll_offset;
    napi_create_object(TaroRuntime::NativeNodeApi::env, &scroll_offset);

    SetNamedProperty(env, scroll_offset, "xOffset", result.xOffset);
    SetNamedProperty(env, scroll_offset, "yOffset", result.yOffset);

    return scroll_offset;
}

napi_value Manager::ScrollTo(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    napi_value node_value = args[0];
    napi_value scroll_params = args[1];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroNode> node = render->document_->GetElementById(nid);
    if (node == nullptr || !node->is_init_) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found or not init.", nid);
        return nullptr;
    }
    auto scroller_node = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroScrollerContainer>(node);
    if (scroller_node == nullptr) {
        TARO_LOG_ERROR("Manager", "node[%{public}d] is not a scroll-view", nid);
        return nullptr;
    }

    Optional<double> xOffset = TaroRuntime::NapiGetter::GetProperty(scroll_params, "xOffset").Double();
    Optional<double> yOffset = TaroRuntime::NapiGetter::GetProperty(scroll_params, "yOffset").Double();
    Optional<u_int32_t> duration = TaroRuntime::NapiGetter::GetProperty(scroll_params, "duration").UInt32();

    double xOffset_;
    double yOffset_;
    if (xOffset.has_value()) {
        xOffset_ = xOffset.value();
    }
    if (yOffset.has_value()) {
        yOffset_ = yOffset.value();
    }

    scroller_node->ScrollTo({xOffset_, yOffset_, duration.value_or(0)});

    return nullptr;
}

napi_value Manager::RegisterFontFamily(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);
    napi_value fontFamilies = args[0];
    napi_valuetype valuetype;
    napi_typeof(env, fontFamilies, &valuetype);
    if (valuetype != napi_object) {
        TARO_LOG_ERROR("Manager", "InitFileManger invalid object param");
        return nullptr;
    }
    auto allPropertyNames = TaroRuntime::NapiGetter::GetAllPropertyNames(fontFamilies);
    for (auto& property : allPropertyNames) {
        std::string fontName = TaroRuntime::NapiGetter(property).StringOr("");
        std::string fontPath = TaroRuntime::NapiGetter::GetProperty(fontFamilies, fontName.c_str()).StringOr("");
        auto fontFamilyManager = TaroRuntime::TaroCSSOM::FontFamilyManager::GetInstance();
        if (!fontName.empty() && !fontPath.empty()) {
            if (FileManager::GetInstance()->PathExists(fontPath)) {
                fontFamilyManager->SetFont(fontName.c_str(), fontPath.c_str());
            }
        } else {
            TARO_LOG_ERROR("Manager", "register font error fontname %{public}s fontPath %{public}s", fontName.c_str(), fontPath.c_str());
        }
    }
    return nullptr;
}

ArkUI_DrawableDescriptor* Manager::GetDrawableDescriptor(std::string& key) {
    auto iter = resourceMap_.find(key);
    if (iter != resourceMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

napi_value Manager::NextTick(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    // 获取第一个参数 callback
    napi_value callback = args[0];
    napi_valuetype valuetype;
    napi_typeof(env, callback, &valuetype);
    if (valuetype != napi_function) {
        TARO_LOG_ERROR("Manager", "Invalid callback function");
        return nullptr;
    }
    napi_ref callback_ref = nullptr;
    napi_create_reference(env, callback, 1, &callback_ref);

    // 监听下一帧布局完成
    TaroRuntime::DirtyTaskPipeline::GetInstance()->RegistryNextTick([arkJs, callback_ref]() mutable {
        std::vector<napi_value> args;
        arkJs.call(arkJs.getReferenceValue(callback_ref), args);
        arkJs.deleteReference(callback_ref);
    });

    return nullptr;
}

napi_value Manager::InitFileManger(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);

    napi_value initObj = args[0];
    napi_valuetype valuetype;
    napi_typeof(env, initObj, &valuetype);
    if (valuetype != napi_object) {
        TARO_LOG_ERROR("Manager", "InitFileManger invalid object param");
        return nullptr;
    }
    auto status = FileManager::GetInstance()->Init(initObj);
    if (status != 0) {
        TARO_LOG_ERROR("Manager", "InitFileManger init fail %{public}d", status);
        return nullptr;
    }
    return nullptr;
}

napi_value Manager::ExecuteNodeFunc(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> node = render->document_->GetElementById(nid);
    if (node) {
        return node->ExecuteFunc(args[1], args[2]);
    }
    return nullptr;
}

napi_value Manager::Unstable_SetPageIsTextNeedLayout(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    napi_value node_value = args[0];
    napi_value is_need_layout_value = args[1];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found.", nid);
        return nullptr;
    }
    bool is_need_layout = arkJs.getBoolean(is_need_layout_value);
    std::shared_ptr<TaroRuntime::TaroDOM::TaroPageContainer> pageContainer = TaroRuntime::TaroDOM::TaroDocument::GetInstance()->findDOMPage(node);
    if (pageContainer) {
        pageContainer->unstable_isTextNeedLayout = is_need_layout;
    }
    return nullptr;
}

napi_value Manager::Unstable_unsafe_SetSwiperOverflow(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    napi_value node_value = args[0];
    napi_value is_hidden_value = args[1];
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(node_value);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found.", nid);
        return nullptr;
    }

    if (auto swiper_node = std::dynamic_pointer_cast<TaroRuntime::TaroDOM::TaroSwiper>(node); swiper_node && swiper_node->is_init_) {
        bool is_hidden = arkJs.getBoolean(is_hidden_value);
        std::static_pointer_cast<TaroRuntime::TaroDOM::TaroSwiperNode>(swiper_node->GetHeadRenderNode())->SetOverflowNow(is_hidden);
    }

    return nullptr;
}

napi_value Manager::LoadImage(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    // 路径
    std::string src = arkJs.getString(args[0]);

    napi_valuetype valuetype;

    // 获取第二个参数 onload callback
    napi_value successCallback = args[1];
    napi_typeof(env, successCallback, &valuetype);
    napi_ref success_callback_ref = nullptr;
    if (valuetype == napi_function) {
        napi_create_reference(env, successCallback, 1, &success_callback_ref);
    }
    // 获取第三个参数 onerror callback
    napi_value errorCallback = args[2];
    napi_typeof(env, errorCallback, &valuetype);
    napi_ref error_call_ref = nullptr;
    if (valuetype == napi_function) {
        napi_create_reference(env, errorCallback, 1, &error_call_ref);
    }
    TaroHelper::loadImage(
        {.url = src.c_str(), .keepPixelMap = true},
        [src, env, success_callback_ref, error_call_ref](const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>& result) {
            ArkJS arkJs(env);
            auto res = std::get_if<TaroHelper::ResultImageInfo>(&result);
            if (res) {
                OH_PixelmapNativeHandle pixelmap = OH_ArkUI_DrawableDescriptor_GetStaticPixelMap(res->result_DrawableDescriptor);
                TaroRuntime::TaroDOM::TaroTmpResource::GetInstance()->tmp_pixels_manager_[src.c_str()] = pixelmap;
                std::vector<napi_value> args(2);
                args[0] = arkJs.createDouble(TaroRuntime::px2Vp(res->width));
                args[1] = arkJs.createDouble(TaroRuntime::px2Vp(res->height));
                arkJs.call(arkJs.getReferenceValue(success_callback_ref), args);
            } else {
                std::vector<napi_value> args;
                arkJs.call(arkJs.getReferenceValue(error_call_ref), args);
            }
            arkJs.deleteReference(success_callback_ref);
            arkJs.deleteReference(error_call_ref);
        });

    return nullptr;
}
} // namespace TaroHarmonyLibrary
