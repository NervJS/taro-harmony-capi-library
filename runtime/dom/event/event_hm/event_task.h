#pragma once

#include "event_base.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class TaroEventTask final {
public:
    TaroEventTask() = default;
    ~TaroEventTask() = default;

    static TaroEventTask* instance();

    void pushHmEvent(TaroEventBasePtr event);
    
    void execEvent();
    
    void OnVsync(uint64_t time, uint64_t id);

private:
    std::list<TaroEventBasePtr> hm_event_queue_;

    // vsync key frame
    std::shared_ptr<TaroRuntime::KeyframeVsync> task_keyframe_ = nullptr;
    bool in_vsync_ = false;
};
}  // namespace TaroRuntime::TaroDOM::TaroEvent
