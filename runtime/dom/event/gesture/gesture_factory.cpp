/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "gesture_factory.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroGestureFactory* TaroGestureFactory::instance() {
    static auto s_instance = new TaroGestureFactory();
    return s_instance;
}

void TaroGestureFactory::registerGesture(int type, GestureEventCreator event_creator, GestureOpCreator op_creator) {
    creators_[type] = {event_creator, op_creator};
}

std::shared_ptr<GestureEventBase> TaroGestureFactory::createGestureEvent(int type) {
    const auto iter = creators_.find(type);
    if (iter == creators_.end()) {
        return nullptr;
    }
    return iter->second.first();
}

GestureOperatorPtr TaroGestureFactory::createGestureOperator(int type) {
    const auto iter = creators_.find(type);
    if (iter == creators_.end()) {
        return nullptr;
    }
    return iter->second.second();
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
