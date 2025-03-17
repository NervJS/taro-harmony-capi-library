//
// Created by wangzhongqiang on 24-4-28.
//
#pragma once

#include <functional>
#include <map>
#include <string>
#include <arkui/native_node.h>

#include "gesture_event_base.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class TaroGestureFactory final {
 public:
    using GestureEventCreator = std::function<std::shared_ptr<GestureEventBase>()>;
    using GestureOpCreator = std::function<GestureOperatorPtr()>;
 public:
    static TaroGestureFactory *instance();
    TaroGestureFactory() = default;
    ~TaroGestureFactory() = default;
    void registerGesture(int type, GestureEventCreator event_creator, GestureOpCreator op_creator);
    std::shared_ptr<GestureEventBase> createGestureEvent(int type);
    GestureOperatorPtr createGestureOperator(int type);
 private:
    std::unordered_map<int, std::pair<GestureEventCreator, GestureOpCreator>> creators_;
};

class TaroGestureEventRegisterHelper {
    public:
    TaroGestureEventRegisterHelper(TaroGestureType event_type,
        TaroGestureFactory::GestureEventCreator event_creator,
        TaroGestureFactory::GestureOpCreator op_creator) {
        TaroGestureFactory::instance()->registerGesture((int)event_type, event_creator, op_creator);
    }
};

// 注册各事件解析函数、序列函数、数据类
#define REGISTER_GESTURE_EVENT(gesture_type, GestureEvent, GestureOperator) \
    static TaroGestureEventRegisterHelper gesture_event_helper##gesture_type( \
        TaroGestureType::gesture_type, \
        [](){return std::make_shared<GestureEvent>();}, \
        [](){return std::make_shared<GestureOperator>();} );

} // namespace TaroRuntime
