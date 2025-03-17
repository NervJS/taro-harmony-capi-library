#pragma once

#include "event_base.h"
#include "helper/sync_queue.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class TaroEventTaskThread final {
public:
    TaroEventTaskThread() = default;
    ~TaroEventTaskThread() = default;

    static TaroEventTaskThread* instance();
    
    int init();

    void pushHmEvent(const TaroEventBasePtr& event);
    
    void execEvent();

private:
    TaroHelper::SyncQueue<TaroEventBasePtr> hm_event_queue_;
    bool has_init_ = false;
};
}  // namespace TaroRuntime::TaroDOM::TaroEvent
