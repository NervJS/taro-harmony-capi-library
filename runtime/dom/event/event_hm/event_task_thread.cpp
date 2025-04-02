/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_task_thread.h"

#include <thread>

#include "helper/Time.h"
#include "helper/api_cost_statistic.h"
#include "runtime/dom/element/element.h"
#include "runtime/render.h"
#include "thread/TaskExecutor.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroEventTaskThread* TaroEventTaskThread::instance() {
    static auto s_instance = new TaroEventTaskThread;
    return s_instance;
}

int TaroEventTaskThread::init() {
    std::thread task_thread([this]() {
        execEvent();
    });
    task_thread.detach();
    return 0;
}

void TaroEventTaskThread::pushHmEvent(const TaroEventBasePtr& event) {
    hm_event_queue_.enqueue(event);
}

void TaroEventTaskThread::execEvent() {
    auto runner = Render::GetInstance()->GetTaskRunner();
    while (true) {
        auto event = hm_event_queue_.dequeue();
        runner->runTask(TaroThread::TaskThread::MAIN, [event]() {
            uint64_t cost = TaroHelper::TaroTime::getCurrentUsTime() - event->time_mark_;
            //             TaroHelper::TimeCostStatistic::instance()->record("event_task", cost);
            event->target_->getEventEmitter()->triggerEvents(event);
        });
    }
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
