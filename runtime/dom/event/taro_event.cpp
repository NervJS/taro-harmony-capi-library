/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "taro_event.h"

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "event_helper.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroEvent::TaroEvent() {}

TaroEvent::~TaroEvent() {
    ArkJS arkJs(NativeNodeApi::env);
    arkJs.deleteReference(napi_ref_);
    napi_ref_ = nullptr;
}

void TaroEvent::SetNapiValue(napi_value event_obj) {
    ArkJS arkJs(NativeNodeApi::env);
    if (napi_ref_ != nullptr) {
        arkJs.deleteReference(napi_ref_);
    }
    napi_ref_ = arkJs.createReference(event_obj);
}

napi_value TaroEvent::GetNapiValue() {
    ArkJS arkJs(NativeNodeApi::env);
    return arkJs.getReferenceValue(napi_ref_);
}

int TaroEvent::recallJS() {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    if (!target_ || !cur_target_) {
        TARO_LOG_ERROR("TaroEvent", "target is %{public}s null", target_ ? "" : "not");
        return -1;
    }

    napi_value event_obj = GetNapiValue();
    if (event_obj == nullptr) {
        // 创建event object
        int ret = EventHelper::instance()->createTaroEventObj(js_event_type_, cur_target_->GetNodeValue(), event_obj);
        if (ret != 0 || event_obj == nullptr) {
            TARO_LOG_ERROR("Event", "createTaroEventObj failed, %{public}d", ret);
            return -2;
        }
        SetNapiValue(event_obj);
        memToJs();
        serializeFun();
    } else {
        memToJs_Partition();
    }

    // 事件回调
    try {
        cur_target_->dispatchNodeEvent(shared_from_this());
        // int ret = EventHelper::instance()->eventHandler(event_obj, js_event_type_, cur_target_->GetNodeValue());
        // if (ret != 0) {
        //     TARO_LOG_ERROR("Event", "EventHelper::eventHandler failed, ret=%{public}d", ret);
        //     return -3;
        // }
    } catch (const std::runtime_error& e) {
        std::string page_path = cur_target_->context_ ? cur_target_->context_->page_path_ : "null";
        std::string err = "page_path(" + page_path + ")\n" + e.what();
        folly::throw_exception(std::runtime_error(err.substr(0, 768)));
    }

    jsToMem_Partition();
    return 0;
}

void read_js_bool(ArkJS& ark, TaroNapiObject& ark_object, const std::string& js_name, bool& bool_value) {
    try {
        auto napi_val = ark_object.getProperty(js_name);
        if (napi_val) {
            if (ark.getType(napi_val) == napi_boolean) {
                bool_value = ark.getBoolean(napi_val);
            }
        }
    } catch (const std::runtime_error& e) {
        TARO_LOG_ERROR("TaroEvent", "read_fun error:%{public}s", e.what());
    }
}

void read_js_string(ArkJS& ark, TaroNapiObject& ark_object, const std::string& js_name, std::string& str_value) {
    try {
        auto napi_val = ark_object.getProperty(js_name);
        if (napi_val) {
            if (ark.getType(napi_val) == napi_string) {
                str_value = ark.getString(napi_val);
            }
        }
    } catch (const std::runtime_error& e) {
        TARO_LOG_ERROR("TaroEvent", "read_fun error:%{public}s", e.what());
    }
}

void TaroEvent::jsToMem_Partition() {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    napi_value event_obj = GetNapiValue();
    if (!event_obj) {
        return;
    }
    ArkJS ark(NativeNodeApi::env);
    TaroNapiObject ark_object(ark, event_obj);
    read_js_bool(ark, ark_object, "bubbles", bubbles_);
    read_js_bool(ark, ark_object, "_stop", stop_);
    read_js_bool(ark, ark_object, "_end", end_);
    read_js_bool(ark, ark_object, "cancelable", cancelable_);
}

void TaroEvent::memToJs_Partition() {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    napi_value event_obj = GetNapiValue();
    if (!event_obj) {
        return;
    }
    ArkJS ark(NativeNodeApi::env);
    TaroNapiObjectBuilder ark_object(NativeNodeApi::env, ark, event_obj);
    ark_object.addProperty("currentTarget", cur_target_->GetNodeValue());
    try {
        ark_object.build();
    } catch (const std::runtime_error& e) {
        TARO_LOG_ERROR("TaroEvent", "build error:%{public}s", e.what());
    }
}

void TaroEvent::memToJs() {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    napi_value event_obj = GetNapiValue();
    if (!event_obj) {
        return;
    }
    ArkJS ark(NativeNodeApi::env);
    TaroNapiObjectBuilder ark_object(NativeNodeApi::env, ark, event_obj);
    ark_object.addProperty("bubbles", bubbles_);
    ark_object.addProperty("cancelable", cancelable_);
    ark_object.addProperty("_stop", stop_);
    ark_object.addProperty("_end", end_);
    ark_object.addProperty("target", target_->GetNodeValue());
    ark_object.addProperty("currentTarget", cur_target_->GetNodeValue());
    try {
        ark_object.build();
    } catch (const std::runtime_error& e) {
        TARO_LOG_ERROR("TaroEvent", "build error:%{public}s", e.what());
    }
}

void TaroEvent::jsToMem() {
    NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
    napi_value event_obj = GetNapiValue();
    if (!event_obj) {
        return;
    }
    ArkJS ark(NativeNodeApi::env);
    TaroNapiObject ark_object(ark, event_obj);

    // 获取type
    read_js_string(ark, ark_object, "type", js_event_type_);
    read_js_bool(ark, ark_object, "bubbles", bubbles_);
    read_js_bool(ark, ark_object, "cancelable", cancelable_);
    read_js_bool(ark, ark_object, "_stop", stop_);
    read_js_bool(ark, ark_object, "_end", end_);
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
