#pragma once

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <native_vsync/native_vsync.h>
#include <stdint.h>

#include "frame_trigger.h"
#include "runtime/keyframe_vsync.h"

namespace TaroRuntime {
namespace TaroAnimate {
class TaroFrameTriggerMaster {
    struct OperateInfo {
        bool is_delete = true;
        uint32_t trigger_id = 0;
        uint32_t xid = 0;
        std::weak_ptr<TaroFrameTrigger> trigger_;
    };
 public:
    static TaroFrameTriggerMaster* instance();

    TaroFrameTriggerMaster();

    ~TaroFrameTriggerMaster();

    void registerTrigger(uint32_t trigger_id,
        std::shared_ptr<TaroFrameTrigger> trigger, uint32_t xid);

    void unregisterTrigger(uint32_t trigger_id, uint32_t xid);

    void onFrame(uint64_t current_time);

    // 页面切换, <xid，is_active>
    void setXComponentActive(
        const std::vector<std::pair<uint32_t, bool>>& xids_to_active);

    void changeXComponentState(uint32_t xid, bool b_active);

 private:
    std::unordered_map<uint32_t, std::weak_ptr<TaroFrameTrigger>> map_triggers_;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> xid_to_triggers_;
    std::unordered_set<uint32_t> active_xids_;
    std::mutex queue_lock_;
    std::list<OperateInfo> operate_queue_;
    std::shared_ptr<KeyframeVsync> keyframe_vsync = nullptr;
    uint32_t vsync_id_ = 0;
};
} // namespace TaroAnimate
} // namespace TaroRuntime
