/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event.h"

#include <folly/dynamic.h>

#include "arkjs/ArkJS.h"
#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "helper/api_cost_statistic.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/event/event_center.h"
#include "runtime/render.h"

namespace TaroHarmonyLibrary {
const std::vector<napi_property_descriptor> Event::desc = {
    {"onEventCenter", nullptr, Event::OnEventCenter, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"offEventCenter", nullptr, Event::OffEventCenter, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"onEventCenterOnce", nullptr, Event::OnEventCenterOnce, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"triggerEventCenter", nullptr, Event::TriggerEventCenter, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"registerNodeEvent", nullptr, Event::RegisterNodeEvent, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"unRegisterNodeEvent", nullptr, Event::UnRegisterNodeEvent, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"triggerNodeEventOnce", nullptr, Event::ListenNodeEventOnce, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"triggerNodeEvent", nullptr, Event::TriggerNodeEvent, nullptr, nullptr, nullptr, napi_default, nullptr},
};

napi_value Event::OnEventCenterOnce(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    auto vec_names = parseEventNames(env, args[0]);
    napi_value callback = args[1];
    napi_value context = args[2];
    if (!callback || vec_names.empty()) {
        TARO_LOG_ERROR("TaroEvent", "call is null or eventName is empty");
        return nullptr;
    }
    for (const auto& event_name : vec_names) {
        if (event_name.empty()) {
            continue;
        }
        TaroRuntime::TaroDOM::TaroEvent::EventCenter::instance()->once(event_name, callback, context);
    }
    return nullptr;
}
napi_value Event::OnEventCenter(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    auto vec_names = parseEventNames(env, args[0]);
    napi_value callback = args[1];
    napi_value context = args[2];
    if (!callback || vec_names.empty()) {
        TARO_LOG_ERROR("TaroEvent", "call is null or eventName is empty");
        return nullptr;
    }
    for (const auto& event_name : vec_names) {
        if (event_name.empty()) {
            continue;
        }
        TaroRuntime::TaroDOM::TaroEvent::EventCenter::instance()->on(event_name, callback, context);
    }
    return nullptr;
}
napi_value Event::OffEventCenter(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    auto vec_names = parseEventNames(env, args[0]);
    napi_value callback = args[1];
    napi_value context = args[2];
    if (!callback || vec_names.empty()) {
        TARO_LOG_ERROR("TaroEvent", "call is null or eventName is empty");
        return nullptr;
    }
    for (const auto& event_name : vec_names) {
        if (event_name.empty()) {
            continue;
        }
        TaroRuntime::TaroDOM::TaroEvent::EventCenter::instance()->off(event_name, callback, context);
    }
    return nullptr;
}
napi_value Event::TriggerEventCenter(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TaroRuntime::NativeNodeApi::env = env;
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);

    auto vec_names = parseEventNames(env, args[0]);
    napi_value event_args = args[1];
    for (const auto& event_name : vec_names) {
        if (event_name.empty()) {
            continue;
        }
        TaroRuntime::TaroDOM::TaroEvent::EventCenter::instance()->trigger(event_name, event_args);
    }
    return nullptr;
}

napi_value Event::ListenNodeEventOnce(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }
    napi_value callback = args[2];
    napi_valuetype valuetype;
    napi_typeof(env, callback, &valuetype);
    if (valuetype != napi_function) {
        TARO_LOG_ERROR("Manager", "Invalid callback function");
        return nullptr;
    }
    napi_ref callback_ref = nullptr;
    napi_create_reference(env, callback, 1, &callback_ref);

    std::string ev_name = arkJs.getString(args[1]);
    node->getEventEmitter()->registerEvent_Once(ev_name, callback_ref);
    return nullptr;
}

napi_value Event::TriggerNodeEvent(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = arkJs.getInteger(args[0]);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }
    node->getEventEmitter()->triggerEvent(env, args[1]);
    return nullptr;
}

napi_value Event::RegisterNodeEvent(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }

    node->addNodeEventListener(args[1], args[2]);
    return nullptr;
}

napi_value Event::UnRegisterNodeEvent(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("Manager", "nid[%{public}d] is not found", nid);
        return nullptr;
    }

    node->removeNodeEventListener(args[1], args[2]);
    return nullptr;
}

std::vector<std::string> Event::parseEventNames(napi_env env, napi_value event) {
    ArkJS arkJs(env);
    napi_valuetype type = arkJs.getType(event);
    if (type == napi_string) {
        std::string event_names = arkJs.getString(event);
        return TaroHelper::StringUtils::split(event_names, ",");
    } else if (type == napi_symbol) {
        folly::throw_exception(std::runtime_error("harmony not support Symbol type."));
    }

#if IS_DEBUG
    folly::throw_exception(std::runtime_error(folly::sformat("eventName is not string type: {}.", static_cast<int>(type))));
#else
    TARO_LOG_ERROR("TaroEvent", "eventName is not string type: %{public}d.", static_cast<int>(type));
#endif
    return {};
}
} // namespace TaroHarmonyLibrary
