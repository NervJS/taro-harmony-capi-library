#pragma once

#include <string>

#include "../taro_event.h"
#include "arkui/native_gesture.h"
#include "gesture_param.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class GestureEventBase : public TaroEvent {
    public:
    GestureEventBase() {
        gen_type_ = EventGeneratorType::Gesture;
    };
    virtual ~GestureEventBase() = default;

    // 填充序列化napi
    virtual void serialize() = 0;
    
    int serializeFun() override;

    void setEvent(ArkUI_GestureEvent* event) {
        event_ = event;
    }

    napi_value getEventObj() {
        return js_event_obj_;
    }

    void setJSEventObj(napi_value js_event_obj) {
        js_event_obj_ = js_event_obj;
    }

    public:
    ArkUI_GestureEvent* event_ = nullptr;
    // 回调回JS的事件
    napi_value js_event_obj_ = nullptr;
};

class GestureOperator {
    public:
    GestureOperator(TaroGestureType event_type)
        : event_type_(event_type) {}
    virtual ~GestureOperator();
    virtual int addGesture(ArkUI_NodeHandle node_handle, GestureParam* param, void* recall_param) = 0;
    virtual int removeGesture() = 0;

    protected:
    ArkUI_NodeHandle node_handle_ = nullptr;
    ArkUI_GestureRecognizer* recognizer_ = nullptr;
    TaroGestureType event_type_;
};
using GestureOperatorPtr = std::shared_ptr<GestureOperator>;
} // namespace TaroRuntime::TaroDOM::TaroEvent
