/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <unordered_map>

#include "event_generator.h"
#include "event_hm/event_base.h"
#include "event_hm/event_types/event_type.h"
#include "gesture/gesture_param.h"
#include "taro_event.h"

namespace TaroRuntime::TaroDOM::TaroEvent {

using EventCallFun = std::function<int(TaroEventPtr)>;
using HMEventCallFun = std::function<int(std::shared_ptr<TaroEventBase>, napi_value&)>;

struct EventListener {
    ArkUI_NodeHandle node_handle_ = nullptr;
    EventCallFun node_event_fun_ = nullptr;
    // 回调类型：true，回调js逻辑； false，只回调TaroElementNode内部逻辑，不回调JS
    bool call_js_ = true;
    EventGeneratorType gen_type_ = EventGeneratorType::UserDefine;
    int event_type_ = -1;

    public:
    bool operator==(const EventListener& other) const {
        return node_handle_ == other.node_handle_ && call_js_ == other.call_js_ && gen_type_ == other.gen_type_ && event_type_ == other.event_type_ && (node_event_fun_.target<int(TaroEventPtr)>() == other.node_event_fun_.target<int(TaroEventPtr)>());
    }
};

struct EventListeners {
    std::vector<EventListener> listeners_;
};

using EventListenersPtr = std::shared_ptr<EventListeners>;

// 事件listen传入的参数，均可选填
struct EventListenParams final {
    bool with_native_bubble_ = false;
    // node_handle_不填默认为RenderHeader的node
    ArkUI_NodeHandle node_handle_ = nullptr;
    // 事件回调，用于事件采集数据&更新node数据
    EventCallFun event_fun_ = nullptr;
    // 事件来源类型，默认自定义
    EventGeneratorType gen_type_ = EventGeneratorType::UserDefine;
    // 对应Taro内事件Type
    int event_type_ = -1;
    void* ext_data_ = nullptr;
};

class TaroEventEmitter final {
    public:
    TaroEventEmitter(TaroElement* node);
    ~TaroEventEmitter();
    // 注册事件监听
    int addEventListener(const std::string& js_event_type, EventListenParams* params = nullptr, bool call_js = true);

    // 取消事件监听
    void removeEventListener(const std::string& js_event_type, bool call_js = true, const ArkUI_NodeHandle& node_handle = nullptr);

    // 触发事件
    int triggerEvents(TaroEventPtr event, bool with_native_bubble = false);

    // 用户自定义事件触发，js回调的
    int triggerEvent(napi_env env, napi_value n_val);

    int registerEvent(EventGeneratorType gen_type, int event_type, const std::string& js_event_type,
                      const EventCallFun& node_event_fun = nullptr,
                      const ArkUI_NodeHandle& node_handle = nullptr, void* args = nullptr);

    int registerEvent_NoCallBack(EventGeneratorType gen_type, int event_type, const std::string& js_event_type,
                                 const EventCallFun& node_event_fun,
                                 const ArkUI_NodeHandle& node_handle = nullptr, void* args = nullptr);
    int registerEvent_NoCallBackWithBubble(int event_type, const std::string& js_event_type, const HMEventCallFun& node_event_fun);
    // 清空某个ArkUI_NodeHandle下所有的事件
    void clearNodeEvent(const ArkUI_NodeHandle& node_handle);

    int unRegisterEventByName(const std::string& js_event_name, const ArkUI_NodeHandle& node_handle = nullptr);

    int unRegisterEventByName_NoCallBack(const std::string& js_event_name,
                                         const ArkUI_NodeHandle& node_handle = nullptr);

    int registerEvent_Once(const std::string& js_event_type, napi_ref& callback_ref);

    EventGeneratorPtr getGenerator(EventGeneratorType gen_type);

    // 兼容之前事件逻辑
    // [[deprecated]]
    int registerEvent(int event_type, const std::string& js_event_type,
                      const HMEventCallFun& node_event_fun = nullptr,
                      const ArkUI_NodeHandle& node_handle = nullptr);

    // [[deprecated]]
    int registerEvent_NoCallBack(int event_type, const std::string& js_event_type,
                                 const HMEventCallFun& node_event_fun,
                                 const ArkUI_NodeHandle& node_handle = nullptr);

    // [[deprecated]]
    int registerEvent_NoCallBack(int event_type, const std::string& js_event_type, void* args = nullptr,
                                 const HMEventCallFun& node_event_fun = nullptr,
                                 const ArkUI_NodeHandle& node_handle = nullptr);

    private:
    bool getType(const std::string& js_event_type, EventGeneratorType& gen_type, int& event_type);

    EventGeneratorPtr createGenerator(EventGeneratorType gen_type);

    // 绑定listener上的事件
    int bindEvent(TaroElementRef element, const std::string& js_event_type);

    // 分发JS事件
    int dispatchEvents(TaroEventPtr event);

    // 分发no callback事件, bool是否触发了js回调
    bool dispatchEvents_First(TaroEventPtr event);

    // 判断listener释放还有效
    bool isValid(EventListenersPtr listener, const EventListener& listen);

    private:
    // 管理事件生成、取消等
    std::unordered_map<EventGeneratorType, EventGeneratorPtr> generators_;
    TaroElement* node_owner_ = nullptr;
    std::unordered_map<std::string, EventListenersPtr> listeners_;
};
using TaroEventEmitterPtr = std::shared_ptr<TaroEventEmitter>;
} // namespace TaroRuntime::TaroDOM::TaroEvent
