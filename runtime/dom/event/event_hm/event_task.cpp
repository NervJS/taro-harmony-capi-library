#include "event_task.h"
#include "thread/TaskExecutor.h"
#include "runtime/render.h"
#include "helper/Time.h"
#include "helper/api_cost_statistic.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroEventTask* TaroEventTask::instance() {
    static auto s_instance = new TaroEventTask;
    return s_instance;
}

void TaroEventTask::pushHmEvent(TaroEventBasePtr event) {
    hm_event_queue_.push_back(event);

    if (!in_vsync_) {
        in_vsync_ = true;
        task_keyframe_ = TaroRuntime::KeyframeVsync::getKeyFrameVsync("task_event");
        task_keyframe_->attachOneVsync(
            [this](uint64_t time, uint64_t id) {
                    OnVsync(time, id);
                }
            );  
    }
}

void TaroEventTask::OnVsync(uint64_t time, uint64_t id) {
    std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
    runner->runSyncTask(TaroThread::TaskThread::MAIN, [this]() {
        execEvent();
    });
}

void TaroEventTask::execEvent() {
    in_vsync_ = false;
    while(!hm_event_queue_.empty()) {
        auto event = hm_event_queue_.front();
        hm_event_queue_.pop_front();
        uint64_t cost = TaroHelper::TaroTime::getCurrentUsTime() - event->time_mark_;
        TaroHelper::TimeCostStatistic::instance()->record("event_task", cost);
        event->target_->getEventEmitter()->triggerEvents(event);
    }
}
}  // namespace TaroRuntime::TaroDOM::TaroEvent
