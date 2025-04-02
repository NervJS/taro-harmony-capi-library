/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_center.h"

#include "arkjs/Scope.h"
#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
EventCenter::CallBackInfo_JS::~CallBackInfo_JS() {
    ArkJS ark_js(NativeNodeApi::env);
    if (call_fun_ != nullptr) {
        ark_js.deleteReference(call_fun_);
        call_fun_ = nullptr;
    }
    if (context_ != nullptr) {
        ark_js.deleteReference(context_);
        context_ = nullptr;
    }
}

EventCenter* EventCenter::instance() {
    static auto s_instance = new EventCenter;
    return s_instance;
}

void EventCenter::on(const std::string& event_name, EventCallBackFun callback, void* context) {
    addListen(event_name, callback, context, false);
}

void EventCenter::on(const std::string& event_name, napi_value callback, napi_value context) {
    addListen(event_name, callback, context, false);
}

void EventCenter::off(const std::string& event_name, EventCallBackFun callback, void* context) {
    auto calls_iter = callbacks_.find(event_name);
    if (calls_iter == callbacks_.end()) {
        return;
    }

    std::erase_if(*calls_iter->second, [&callback, &context](const std::shared_ptr<CallBackInfo>& info) {
        if (!info->is_js) {
            auto capi_info = std::static_pointer_cast<CallBackInfo_CAPI>(info);
            return (capi_info && (!callback || capi_info->call_fun_ == callback) && (!context || capi_info->context_ == context));
        }
        return false;
    });

    if (calls_iter->second->empty()) {
        callbacks_.erase(calls_iter);
    }
}

bool compare_napi_value(napi_ref left, napi_value right) {
    if (left == nullptr || right == nullptr) {
        return left == nullptr && right == nullptr;
    }
    bool b_result = false;
    ArkJS ark_js(NativeNodeApi::env);
    napi_strict_equals(NativeNodeApi::env, ark_js.getReferenceValue(left), right, &b_result);
    return b_result;
}

void EventCenter::off(const std::string& event_name, napi_value callback, napi_value context) {
    auto calls_iter = callbacks_.find(event_name);
    if (calls_iter == callbacks_.end()) {
        return;
    }

    std::erase_if(*calls_iter->second, [&callback, &context](const std::shared_ptr<CallBackInfo>& info) {
        if (info->is_js) {
            auto js_info = std::static_pointer_cast<CallBackInfo_JS>(info);
            return (js_info && (!callback || compare_napi_value(js_info->call_fun_, callback) && (!context || compare_napi_value(js_info->context_, context))));
        }
        return false;
    });

    if (calls_iter->second->empty()) {
        callbacks_.erase(calls_iter);
    }
}

void EventCenter::once(const std::string& event_name, EventCallBackFun callback, void* context) {
    addListen(event_name, callback, context, true);
}

void EventCenter::once(const std::string& event_name, napi_value callback, napi_value context) {
    addListen(event_name, callback, context, true);
}

void EventCenter::trigger(const std::string& event_name, napi_value n_val) {
    auto data = std::make_shared<EventCenterData>();
    data->setJsValue(n_val);
    trigger(event_name, data);
}

void EventCenter::trigger(const std::string& event_name, std::shared_ptr<EventCenterData> data) {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    auto iter = callbacks_.find(event_name);
    if (iter == callbacks_.end()) {
        return;
    }
    auto callback_infos = iter->second;
    auto tmp_list = *callback_infos;
    for (auto callback : tmp_list) {
        auto iter_list = std::find(callback_infos->begin(), callback_infos->end(), callback);
        if (iter_list == callback_infos->end()) {
            TARO_LOG_DEBUG("EventCent", "event listen has free");
            continue;
        }
        if (callback->is_js) {
            auto callback_js = std::dynamic_pointer_cast<CallBackInfo_JS>(callback);
            auto napi_val = data->memToJs();
    
            ArkJS arkJs(NativeNodeApi::env);
            std::vector<napi_value> vec_napi_value;
            uint32_t length = arkJs.getArrayLength(napi_val);
            for (uint32_t i = 0; i < length; ++i) {
                napi_value element = arkJs.getArrayElement(napi_val, i);
                vec_napi_value.push_back(element);
            }
            callbackJS(callback_js->call_fun_, callback_js->context_, vec_napi_value);
        } else {
            auto callback_capi = std::dynamic_pointer_cast<CallBackInfo_CAPI>(callback);
            callback_capi->call_fun_(data, callback_capi->context_);
        }
        if (callback->is_once) {
            callback_infos->erase(iter_list);
        }
    }
}

void EventCenter::addListen(const std::string& event_name, EventCallBackFun callback, void* context, bool once) {
    if (event_name.empty() || callback == nullptr) {
        TARO_LOG_ERROR("TaroEvent", "name:%{public}s callback is %{public}s null",
                       event_name.c_str(), callback ? "not" : "");
        return;
    }

    auto call_info = std::make_shared<CallBackInfo_CAPI>();
    call_info->is_js = false;
    call_info->call_fun_ = callback;
    call_info->context_ = context;
    call_info->is_once = once;

    CallBackInfosPtr& callback_infos = callbacks_[event_name];
    if (callback_infos == nullptr) {
        callback_infos = std::make_shared<std::list<std::shared_ptr<CallBackInfo>>>();
    }

    callback_infos->emplace_back(std::move(call_info));
}

void EventCenter::addListen(const std::string& event_name, napi_value callback, napi_value context, bool once) {
    if (event_name.empty() || callback == nullptr) {
        TARO_LOG_ERROR("TaroEvent", "name:%{public}s callback is %{public}s null",
                       event_name.c_str(), callback ? "not" : "");
        return;
    }
    auto call_info = std::make_shared<CallBackInfo_JS>();
    call_info->is_js = true;
    ArkJS ark_js(NativeNodeApi::env);
    call_info->call_fun_ = ark_js.createReference(callback);
    call_info->context_ = ark_js.createReference(context);
    call_info->is_once = once;

    CallBackInfosPtr& callback_infos = callbacks_[event_name];
    if (callback_infos == nullptr) {
        callback_infos = std::make_shared<std::list<std::shared_ptr<CallBackInfo>>>();
    }

    callback_infos->emplace_back(std::move(call_info));
}

void EventCenter::callbackJS(napi_ref ref_call_fun, napi_ref ref_context, std::vector<napi_value> vec_data) {
    ArkJS ark_js(NativeNodeApi::env);
    napi_value n_call_fun = ark_js.getReferenceValue(ref_call_fun);
    if (!n_call_fun) {
        TARO_LOG_ERROR("TaroEvent", "call_fun is null");
        return;
    }
    napi_value n_context = nullptr;
    if (!ref_context) {
        n_context = ark_js.getReferenceValue(ref_context);
    }
    ark_js.call(n_call_fun, vec_data, n_context);
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
